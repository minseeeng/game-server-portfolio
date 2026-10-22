#pragma once

#define SERVERPORT 5000

//-----------------------------------------------------------------
// 프레임당 이동 속도
//-----------------------------------------------------------------
#define MOVE_SPEED_X 3
#define MOVE_SPEED_Y 2

//-----------------------------------------------------------------
// 이동 오류체크 범위
//-----------------------------------------------------------------
#define dfERROR_RANGE		50

//---------------------------------------------------------------
// 화면 이동 영역
//---------------------------------------------------------------
#define dfRANGE_MOVE_TOP	50
#define dfRANGE_MOVE_LEFT	10
#define dfRANGE_MOVE_RIGHT	630
#define dfRANGE_MOVE_BOTTOM	470

//---------------------------------------------------------------
// 공격범위
//---------------------------------------------------------------
#define dfATTACK1_RANGE_X		80
#define dfATTACK2_RANGE_X		90
#define dfATTACK3_RANGE_X		100
#define dfATTACK1_RANGE_Y		30
#define dfATTACK2_RANGE_Y		30
#define dfATTACK3_RANGE_Y		40

#define dfACTION_IDLE			99

#define GAME_FRAME_PER_SEC		50

//---------------------------------------------------------------
// 세션 버퍼 / 동시 접속
//---------------------------------------------------------------
#define SEND_BUFFER_SIZE		10000
#define MAX_SESSION				(FD_SETSIZE - 1)	// select 한 번에 listen 소켓 1칸을 뺀 만큼
