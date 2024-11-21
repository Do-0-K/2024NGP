//=========================================================
// ENUM.h		열거자 정의 헤더
//=========================================================
#pragma once

// 현재 게임의 상태, 렌더링 할때 상태 체크
enum GAME_STATE{타이틀, 메인, 아이템선택, 필드, 결과창};

// 장착한 아이템을 확인할 때 사용(게임 시작 전 아이템)
enum EQUIP_ITEM{김도영의_자켓, 김도영의_핸드폰, 등등};

// 무기별 번호, 추가하기---
enum WEAPONS{나이프, 권총, 라이플};

enum ZOMBIE_TYPE{일반, 힐러, 탱크, 폭발, 사람};