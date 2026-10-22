# TCPFighter 서버 부하 확인용 더미 클라이언트
# 사용법: python dummy_client.py <접속 수>
#   1) N개 접속 -> 각자 자기 캐릭터 생성 패킷을 받았는지, 패킷 스트림이 깨지지 않았는지 확인
#   2) 전원이 이동 시작 패킷을 1번씩 보냄 -> 다른 클라들에게 브로드캐스트가 몇 개 도착했는지 확인
import socket, struct, time, sys, select

PORT = 5000
PACKET_CODE = 0x89
SC_CREATE_MY_CHARACTER = 0
CS_MOVE_START = 10
SC_MOVE_START = 11

N = int(sys.argv[1])
socks = []
for i in range(N):
    s = socket.create_connection(("127.0.0.1", PORT))
    s.setblocking(False)
    socks.append(s)
    time.sleep(0.02)

alive = list(socks)
closed = set()
buf = {s: b"" for s in socks}


def pump(sec):
    end = time.time() + sec
    while time.time() < end and alive:
        r, _, _ = select.select(alive, [], [], 0.1)
        for s in r:
            try:
                d = s.recv(65536)
            except (BlockingIOError, ConnectionError):
                continue
            if not d:  # 서버가 끊음 (접속 거절)
                alive.remove(s)
                closed.add(s)
                continue
            buf[s] += d


# [code 1][size 1][type 1][payload size] 로 자른다. code 가 0x89 가 아니면 스트림이 깨진 것
def parse(b):
    packets = []
    while len(b) >= 3:
        code, size, typ = struct.unpack_from("<BBB", b)
        if code != PACKET_CODE:
            return packets, True
        if len(b) < 3 + size:
            break
        packets.append((typ, b[3:3 + size]))
        b = b[3 + size:]
    return packets, False


pump(2)
my = {}
broken_join = 0
for i, s in enumerate(socks):
    packets, broken = parse(buf[s])
    broken_join += broken
    for typ, p in packets:
        if typ == SC_CREATE_MY_CHARACTER and len(p) >= 9:
            my[i] = struct.unpack_from("<IBhh", p)  # id, dir, x, y

for s in socks:
    buf[s] = b""
for i, s in enumerate(socks):
    if i in my and s not in closed:
        _, _, x, y = my[i]
        s.send(struct.pack("<BBBBhh", PACKET_CODE, 5, CS_MOVE_START, 4, x, y))
pump(2)

received = 0
broken_after = 0
for s in alive:
    packets, broken = parse(buf[s])
    broken_after += broken
    received += len({struct.unpack_from("<I", p)[0] for t, p in packets if t == SC_MOVE_START})
expected = len(my) * (len(my) - 1)

print(f"접속 {N} / 거절 {len(closed)} / 캐릭터 생성 {len(my)} / 입장 시 스트림 깨짐 {broken_join}"
      f" / 이동 브로드캐스트 {received}/{expected} ({100 * received / max(expected, 1):.0f}%)"
      f" / 이후 스트림 깨짐 {broken_after}")

for s in socks:
    s.close()
