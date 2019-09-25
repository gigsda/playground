# import-rdb-webui
## 프로젝트 목적
[Import 배치 작업](#Import_RDB_배치) 의(https://github.com/music-flo/reco-workflow/tree/master/import-rdb) 대상 테이블을 WebUI를 통해 관리합니다.
 

## Requirements
You need Python 3.7.3 or later 

## Quick Start
### Setup
```
pip install -r requirements.txt
```

### Run(dev)
```
# 설정 파일 검토
vi import_rdb_webui/import_rdb_webui/settings.py

# 코드로 정의한 세션, 사용자, 모델, 어플리케이션 등 내부 관리 데이터 스키마를 DB(현재 설정은 로컬 sqlite3)에 적용합니다.
python import_rdb_webui/manage.py makemigrations imports
python import_rdb_webui/manage.py migrate

# 서버를 구동합니다 
python import_rdb_webui/manage.py runserver &

# 배치 작업을 등록 합니다(DB 및 파라메터 스토어 데이터 갱신)
# 주의: 시스템의 crontab 에 등록 합니다.
python manage.py crontab add

```

### connect(dev)
http://{{ user_host }}:{{ user_port }}/
 
### Shutdown
```
# kill 'manage.py runserver' process

# 배치 작업을 등록 해재 합니다.
python manage.py crontab remove
```

## 구조 및 주요 컴포넌트 설명

### 용어
#### Import RDB 배치<a name="Import_RDB_배치"></a>
  - 외부의 배치 작업으로 'Parameter Store' 에 저장된 테이블 정보 목록을 가져와 Import 작업을 수행 합니다.
#### Parameter Store
  - AWS Parameter Store 를 말하며, 테이블 정보 목록을 가지고 있습니다. 
  - CSV 형태로 저장하며 category_name, table_name, num_mappers, comment, split_col, db_env 컬럼을 가지고 있습니다.
  - Parameter Store는 8124KB 용량 제약이 있어, zip으로 압축하고 base64로 인코딩해 저장합니다.
#### 소스 RDB 데이터베이스
  - 데이터를 가져올(Import) MCP, POC 등의 RDB 데이터 베이스를 말합니다.
#### Import Table
  - Import할 대상인 소스 RDB 데이터베이 있는 테이블입니다.
[here](#place-2)

### 주요 컴포넌트
 - WebUI 
   - 'Import 요청' (http://{{ user_host }}:{{ user_port }}/imports/import_rdb_table/)
     - Import 대상 테이블을 추가/수정/삭제 합니다(Reco 그룹 사용자).
     - Import 대상 테이블을 추가/수정/삭제를 요청합니다(User 그룹 사용자).
   - '요청 내역' (http://{{ user_host }}:{{ user_port }}/imports/request_import_rdb_table/) 
     - User 그룹 사용자가 요청한 요청 목록을 보여주고, Reco 그룹 사용자는 이곳에서 요청을 적용/거절 할 수 있습니다.
 
 - 데이터 스토어
   - 파라메터 스토어
     RDB -> AWS 전송할 테이블 목록을 저장합니다. 별도의 배치가 이 저장소의 테이블 목록을 가져와 Import 작업을 수행 합니다.
   - 내부 DB 엔진
     사용자가 수정한 테이블 목록을 가지고 있습니다. 수정한 Row의 status 값은 'Draft' 가 되며 주기적으로 '파라메터 스토어'에 업로드 합니다.
   - 소스 RDB
     Import 할 테이블을 가진 DB 입니다.
 - Cron 배치
   - 주기 적으로 병합 작업을 트리거링 하는 용도로 사용합니다.
   
## 소스 이해를 위해
Django Admin 기반으로 만들었습니다.
데이터 모델만 정의 코드만 작성하면, 데이터 추가/수정/삭제가 가능한 기본 어드민이 만들어지고, 필요에 따라 컴포넌트를 재정의하는 방식으로 개발하기에  Django에 대한 사전 지식 없이 코드만 보고 동작을 바로 이해하기에는 무리가 있습니다만, 조금만 시간을 투자한다면 어렵지 않게 이해할 수 있습니다.

 - 직접 해보지 않고 최소한의 시간으로 Django 어드민 기반 앱 작성을 이해하는 데 도움이 되는 슬라이드입니다.(추천)
   - https://www.slideshare.net/bbayoung7849/djangoadminsitecustomexample

 - 공식 튜토리얼입니다. part 1,2 까지만 봐도 소스를 이해하는데 충분합니다.
   - https://docs.djangoproject.com/en/2.1/intro/tutorial01/
 

     
 ### Place 2
