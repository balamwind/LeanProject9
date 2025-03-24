# [9번 과제] 숫자 야구 게임

9번 과제인 숫자 야구 게임을 만들었습니다.<br>
언리얼 엔진에서 독립형 게임으로 실행시키면 됩니다.<br>
첫 턴의 플레이어는 접속한 클라이언트며, 서버는 다음 턴입니다.
<br>
<br>

다음과 같이 게임 플레이가 진행됩니다.

![제목 없는 다이어그램](https://github.com/user-attachments/assets/010e404b-c77a-42f5-b827-b8eee97d9fb2)

<br>

아래와 같이 겹치지 않게 문제를 설정하게 됩니다.

![image](https://github.com/user-attachments/assets/93199057-c80a-4de3-bdf3-a6d63aa06f17)

아래처럼 정답이 전해지면 Strike와 Ball을 판단해 정답인지 아닌지 판단 후<br>
정답이라면 각 플레이어에게 Win과 Lose가 전해지고 게임이 종료됩니다.<br>
정답이 아닐 시 턴이 남아있다면 다음 플레이어의 턴이 되고, 턴이 없다면 게임이 종료됩니다.

![image](https://github.com/user-attachments/assets/88bd2eec-2378-441f-8090-f6ba0214c9e4)

아래와 같이 ACPlayerController에 결과가 전달됩니다.

![image](https://github.com/user-attachments/assets/e5814094-9bed-46a9-8283-99e5b245df21)

입력에 관련된 오류 사항은 Wiget Blueprint에서 처리해 서버에 OUT 처리로 전달합니다.

![image](https://github.com/user-attachments/assets/67c1cf66-7f10-4cd3-a5d4-6d3653650cc1)

턴이 시작 된다면 타이머가 실행되어 화면에 자연스럽게 보이게 됩니다. <br>
단, 타이머가 모두 지났을 때, 턴 플레이어를 OUT 처리로 하는 것은 서버에서만 실행됩니다.

![image](https://github.com/user-attachments/assets/6c102742-89ac-4cab-b319-5a058a83b06c)

---

## 과제 발제

### 1️⃣ Intro :  숫자 야구 게임을 만들어 봅시다.

비디오 게임의 할아버지의 할아버지로 만들어진지 100년이 넘는 숫자 야구 게임을 만들어 봅시다. 원조 게임은 4자리 숫자로 하는 Bulls and Cows 입니다.

규칙은 간단합니다.
* 처음 접속하면 서버가 1~9까지 겹치지 않는 3자리의 숫자를 만듭니다.
* Host 와 Guest 는 차례대로 “/123”, “/543” 와 같이 겹치지 않는 3자리 숫자를 답합니다.
* 서버가 생성한 랜덤한 숫자와 사용자가 입력한 숫자를 비교하여 결과를 반환합니다.
  * 자리수와 값이 같으면 스트라이크(S) 숫자를 늘립니다.
  * 자리수는 다르고 값이 같으면 볼(B) 숫자를 늘립니다.
  * 예 1. 서버 생성 숫자 386, 사용자 답변 127 ⇒ OUT
  * 예 2. 서버 생성 숫자 386, 사용자 답변 167 ⇒ 0S1B
  * 예 3. 서버 생성 숫자 386, 사용자 답변 367 ⇒ 1S1B
  * 예 4. 서버 생성 숫자 386, 사용자 답변 396 ⇒ 2S0B
  * 예 4. 서버 생성 숫자 386, 사용자 답변 386 ⇒ 3S0B ⇒ Win
* 한쪽 플레이어가 3번 만에 3S를 못하면, 남은 플레이어가 Winner가 됩니다.
* 3S 를 먼저 맞추는 플레이어가 Winner가 됩니다.
* Winner가 나오면 게임은 리셋됩니다. (출력메시지는 “Host Won!! 다시 게임이 시작됐다.” 또는 “Guest Won!! 다시 게임이 시작됐다.”)
* 둘 다 3번만에 못 맞추면 게임은 리셋됩니다. (출력메시지는 “무승부군. 다시 게임을 시작하지”)

로직이 조금 복잡해서 BP로 하기에는 굉장히 길어집니다만, 이러한 로직을 C++로 작성하면 길지 않은 크기로 작성할 수 있습니다.<br> 
실무환경에서 자주 만나는 문제로 Blueprint의 복잡성을 단순화 시키고, 게임 성능을 높이기 위해 Blueprint로 만들어진 거대한 코드를 C++ 라이브러리로 재작성하는 흔한 시나리오입니다.

### 3️⃣ 필수 기능 가이드

* GameMode 서버 로직
  * 채팅 기반으로 명령어를 입력하면 서버가 이를 해석해서 게임 로직을 처리해야 합니다.
  * 예: “/123” 형태로 입력되었을 때, 게임 진행 상황(스트라이크/볼 계산, 시도 횟수, 승리/패배/무승부)을 판정합니다.

* 3자리 난수 생성 로직 (중복 없는 1~9 숫자)
  * 게임 시작 시 서버에서 무작위 3자리 숫자를 만듭니다.
  * 예: 386, 594 등(0은 포함하지 않음, 중복 없음).

* 판정 로직 (S/B/OUT 계산)
  * 서버가 생성한 숫자와 플레이어가 입력한 숫자를 비교하여 스트라이크(S), 볼(B), 아웃(OUT)을 계산합니다.
  * 자리수와 값이 같으면 S, 자리수는 다르지만 값이 존재하면 B, 둘 다 아니면 OUT.

* 시도 횟수 및 상태 관리
  * 플레이어(Host/Guest) 각각 3번의 기회를 가집니다.
  * 유효하지 않은 입력을 했을 때(예: 중복 숫자 포함, 3자리가 아닌 입력 등)는 아웃(OUT) 처리.
  * 입력할 때마다 플레이어별 남은 시도 횟수를 갱신합니다.

* 승리, 무승부, 게임 리셋
  * 3번 내에 3S를 맞춘 플레이어가 나오면 즉시 승리 처리.
  * 플레이어가 아웃되었을 때, 남은 플레이어가 자동으로 승리.
  * 두 플레이어 모두 3번씩 시도를 끝내도 승자가 없으면 무승부 처리.
  * 승리 혹은 무승부 시, 게임을 재시작(숫자 및 시도 횟수 리셋).

* C++ 라이브러리 분리
  * 판정(S/B 계산)과 난수 생성 로직은 별도의 C++ 라이브러리(또는 C++ Class)로 분리해서 구현합니다.
  * GameMode에서는 이 라이브러리를 호출하여 결과를 받아온 후 게임 전체 흐름(채팅 처리, 시도 횟수, 승패 결정 등)을 관리합니다.

### 4️⃣ 도전 기능 가이드 

* 턴(Turn) 제어 기능
  * 고스톱이나 하스스톤처럼 특정 턴이 끝나야 다른 플레이어가 입력할 수 있게 만들어주세요.
  * 혹은, 일정 시간이 지나면 턴이 자동으로 넘어가도록 구현하셔도 좋습니다.

* 게임성 고도화
  * 제한 시간(Timer)을 두고, 시간 내에 입력을 못 하면 기회를 소진하게 만들어보세요.
  * 누가 몇 번의 승리를 가져갔는지 스코어보드를 보여주는 기능을 추가해주세요.
