
----버전1.0----
■ 노트 판정
 - 노트 판정의 세분화(O)
 -> 문제: 다른 판정이 떠도 애니메이션 잔상(완료)


☆버그: Perfect 인데 Great 치면 Perfect 효과 다 안사라졌는데 Great 겹쳐서 출력
      Sprite를 포인터를 받아서 해결할것(완료)

■ 콤보 시스템
 - 현재 콘솔에서 콤보 출력중


----버전2.0----
■ 멀티트랙

버그 : 입력으로 인한 MISS 판정 때, 콤보 0 으로 초기화 되었지만
       치지 못해서 MISS난 경우, 콤보가 0으로 초기화 되지 않는 버그(완료)


■ 노트 판정에 따른 이펙트
- 리팩토링
 TrackManager, Track 클래스 스프라이트 제거 코드 중복
 판정이펙트 가운데 정렬, 노트 그냥 PASS 되었을 시 미스이펙트, 콤보처리

- 절대좌표에서 상대좌표로 옮길것(O)
- 콤보 폰트 출력 완료
- TrackManager 리팩토링
  가독성, 자료구조로 변수 관리
- 스코어 시스템 추가
- Pause 기능 추가(중지: F4, 재개: F5)
- ResourceManager 추가 - 로딩시간 최적화
- 음악 볼륨 증감 추가

----버전2.1----
- 버그: 볼륨, Pause & Resume 키 입력시 콤보 올라가는 현상(완료)

- main.cpp 키입력 중복코드 삭제 -> GameScene에서 키입력을 처리
- GameScene으로 정지, 볼륨제어기능 이동
- SceneManager 생성. SceneManager에서 Scene 생성 제거 Change 관리
- 로고씬, 타이틀씬 추가

☆- 결과씬 미구현

----버전3.0----
- 버그: 진행 중 ESC로 타이틀 가도 음악 중지 안되는 현상(완료)
- 1Key 4Key 선택 기능 삭제하고 4key로 고정(불필요하다고 느낌)
- 스프라이트 Stop 기능 추가

- TrackManager TrackSetting csv 파싱부분 추가할 것
- 게임 끝나고 결과화면 대신 타이틀화면으로 가는 중(임시)


■ 리팩토링 필요(TrackManager 판정 효과 중복 코드)

■ 롱노트
 - 판정 추가(Keyup 부분) : 롱노트에서 키를 떼는 순간(Keyup) 미스판정이 안남
	-미스판정
	 롱노트 끝나는데 키를 안뗐을 경우
	 롱노트가 끝나기 전에 키를 뗐을 경우
	-완료

 - 콤보 : TrackManager에서 콤보를 올리고 있는데 공유자원 접근 문제
	Score와 콤보를 관리하는 Manager 클래스 생성(완료)

----버전3.1----

■ 판정 수정 및 이펙트 수정
 - 트랙매니저에서 각 트랙의 판정을 받아 이펙트를 출력시키는데 트랙매니저의 역할이 아님
 - MISS GREAT PERFECT 등의 이펙트를 트랙에서 관리할 수 있게 수정할 것(완료)
 - 키 타입 추가: NEUTRAL, PRESS, HOLDING(완료)
 - 리팩토링: 리네이밍
	     TrackManager 판정 효과 중복 코드 (완료)
 - 콤보 : 롱노트 유지시 200틱 간격으로 콤보 오르게 할 것 (약간 깔끔하게 오르지 않음)
	  1초면 5콤보
 - 볼륨값 유지되도록 GameSystem에서 볼륨값 컨트롤(완료)
 - 버그 : Pass 되서 치지 못한 노트가 있을 때, 콤보 폰트가 바로 갱신되지 않고 다음 노트칠 때 갱신되는 버그(완료)
 - 노트를 트랙에서 독립적으로 생성(차후에 BMS를 위해)


----버전4.0----
■ BMS
1. 잘못 이해한 것
- #00114:ZZ 한 마디가 롱노트 -> 틀림
- #00115:0V 롱노트 키가 아니지만 한마디를 차지하는 노트는 롱노트로 -> 틀림	
	그냥 마디의 첫번째 위치에 노트 하나 배치
	
2. GameScene에서 게임정보를 가져오는 csv 파일 수정 - PlayTime 삭제
- TotalPlayTime은 트랙매니저에서 bms 파싱하고 결정한다.
		
3. 키입력은 되는데 판정이 안됨?(버그)
- 노트배치는 먼저 전체 플레이타임이 정해진 상태에서 계산됨.
- 현재 그렇게 되지 않고 있음.
- 트랙쪽 Init도 마찬가지
- 추가로 역방향으로 노트를 넣었는데 추가 함수에서는 앞에서부터 넣고 있었다..

4. 노트 밀리는 현상(미수정)
- 노트를 판정 후에 노트를 제거하는데 제거하면서 Update순서 변동으로 인해 deltaTime이 일정하게 들어가지 않아서 그렇다.


----버전4.1----
■ 직접 만들었던 자료구조를 STL로 변환 중
 - 직접 만들었던 Array 자료구조를 vector로 교체 중.
 - 직접 만들었던 DoubleLinkedList도 list로 교체 중
 - 사용하는 곳이 나중 업데이트 때 가변배열을 필요로 할 가능성이 높음
 - 수정완료된 곳: Sprite, EffectPlayer
 - 진행 중: Trackmanager 이중 연결 리스트 수정중
 
 
----버전4.2----
■ 키 변경 시스템 제작 중

남은 기능
■ 피버 시스템
■ 노트별 효과음(파싱하면서 해결될 요소)
■ 스프라이트 애니메이션 속도
■ 다른 리소스들 모아서 선택해서 실행(같이 하는 사람들의 리듬게임 구조와 파싱구조가 많이 달라져 개인적으로 만들기로 했음)