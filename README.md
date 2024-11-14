# 2024 2학기 컴퓨터 그래픽스 Polygon Slice

#### 2022180024 유영빈

### 실행 시 알아야할 요소
 - Bullet Physics 사용
   1. Visual Studio에서 프로젝트 > 구성 > C/C++ > 추가 포함 디렉터리 > include 폴더 추가
   2. Visual Studio에서 프로젝트 > 구성 > 링커 > 추가 라이브러리 디렉터리 > lib 폴더 추가
   3. Visual Studio에서 프로젝트 > 구성 > 링커 > 입력 > 추가 종속성 > BulletCollision.lib;BulletDynamics.lib;LinearMath.lib 추가

![설정화면](/img/img_4.png "설정화면")

### 단축키 (Press 상태이어야 함)
* 's' key - down : 게임 시작
* '1' key - down | up : 도형 LINE | 도형 FILL
* '2' key - down | up : 도형의 이동 경로 (미래 및 과거) 출력 || 출력 X
* '-' key - down : 도형의 이동속도 증가 ( limit : 3.00 )
* '+' key - down : 도형의 이동속도 감소 ( limit : 0.01 )
* '[' key - down : 도형의 생성 속도 증가 ( limit : 60ms )
* ']' key - down : 도형의 생성 속도 감소 ( limit : 300ms )
* '방향키 상, 하, 좌, 우' key - down : 카메라를 0, 0, 0을 중심으로 상, 하, 좌, 우 회전
* '<', '>' key - down : 카메라를 0, 0, 0을 중심으로 x축 좌 우 이동
* ':', '"' key - down : 카메라를 0, 0, 0을 중심으로 z축 확대 축소 이동
* 'q' key - down : 게임 종료

### 마우스 (Press 상태이어야 함)
* 마우스 좌클릭 - down | up : 드래그에 따른 도형 생성 (슬라이스)

### 구현한 내용 & 추가 구현 내용
* 모델링이 날아오는 경로 설정
  * 지정된 z값을 기준으로 모델이 카메라를 향해 다가오며 중심을 기준으로 아래로 떨어짐
* 바구니가 좌우로 이동
* 마우스 드래그를 통한 도형 슬라이스
  * 슬라이스 된 도형은 아래로 떨어짐
  * 떨어진 도형은 바구니에 닿게 되면 바구니 위에 놓여짐
* 슬라이스 하지 못하면 아래로 사라짐
---
* 3D 도형 모델 추가
  * 도형의 종류 : 정육면체, 스피어, 실린더, 티폿, 사각형(2D)
* 모든 도형은 x, y, z축을 기준으로 랜덤하게 회전함
* 드래그를 통해서 x,y축 값을 기준으로 지정된 z(깊이)의 평면 사각형을 생성함
  * 생성된 도형의 z축과 움직이는 도형 모델의 z축이 닿을때 충돌 체크 (깊이값)
  * 생성된 사각형과 모델이 충돌하면 도형이 슬라이스됨 (쉬운 버전)
  * 슬라이스 된 모델과 바구니의 z값이 일치한 위치에서 충돌해야됨 (윈도우에서 가운데 쯤에서 드래그 슬라이스)
* '2'를 누르고 있으면 모델의 이동 경로가 출력됨
* 모든 3D 모델에 광원(Pong) 적용 (사각형은 적용했는데 이상함)
* 카메라 이동

![동작화면2](/img/img_2.png "동작화면2")
![동작화면3](/img/img_3.png "동작화면2")
![동작화면1](/img/img_1.png "동작화면1")

### 구현하지 못한 내용
* 드래그한 좌표값에 따른 도형의 자연스러운 슬라이스


