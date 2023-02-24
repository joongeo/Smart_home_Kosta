#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <unistd.h>
#include <mosquitto.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define STEP_OUT1A 27 //GPIO pin num
#define STEP_OUT1B 0
#define STEP_OUT2A 1
#define STEP_OUT2B 24
#define SWITCH 4    //swich pin num  
#define LED 5        //led pin num

#define MQTT_HOSTNAME "192.168.0.12"
#define MQTT_PORT 1883
#define MQTT_USERNAME "admin"
#define MQTT_PASSWORD "admin"
#define MQTT_TOPIC "CRhouse/CURTAIN"
#define MQTT_TOPIC2 "TIME"

char openHour=0,openMin=0;       //사용자가 입력한 시간,분에 대한 변수
char closeHour=0, closeMin=0;    // 닫힐 시간에 대한 변수
int i;							//step moter 시간 변수
int userMin=0, userHour=0;
//시간관련
time_t rawtime;     
struct tm *tm;
struct tm newtime;
char *userTime[10]={NULL,};


//커낵트 됬을때 실행되는 함수, 브로커에서 응답하면 클라이언트를 해당 브로커에 섭스크라이브한다
void on_connect(struct mosquitto *mosq, void *obj, int rc) {
	printf("ID: %d\n", * (int *) obj);
	if(rc) {
		printf("Error with result code: %d\n", rc);
		exit(-1);
	}
	mosquitto_subscribe(mosq, NULL, MQTT_TOPIC2, 0);
	char text2[30] = "yeojin Rasp2\n";
   	mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(text2), text2, 0, false);

/////////////curtainControl//////////////////


    //동작
    time(&rawtime);     //Return the current time and put it in *TIMER if TIMER is not NULL.
    tm=localtime_r(&rawtime, &newtime); //Return the `struct newtime' representation of *TIMER in local time, using *TP to store the result.
    int sw=0;
}

//메세지를 받았을때 실행되는 함수, 매세지를 받을시 그대로 출력한다
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
	//printf("New message with topic %s: %s\n", msg->topic, (char *) msg->payload);
   
    char *ptr = strtok(msg->payload, ",");      // "," 를 기준으로 문자열을 자름, 포인터 반환

    while (ptr != NULL)            // 자른 문자열이 나오지 않을 때까지 반복
    {
        userTime[i] = ptr;             // 문자열을 자른 뒤 메모리 주소를 문자열 포인터 배열에 저장
        i++;                       // 인덱스 증가
        ptr = strtok(NULL, ",");   // 다음 문자열을 잘라서 포인터를 반환
    }
    ptr = NULL;

    for (int i = 0; i < 10; i++)
    {
        if (userTime[i] != NULL)           // 문자열 포인터 배열의 요소가 NULL이 아닐 때만
            printf("%s\n", userTime[i]);   // 문자열 포인터 배열에 인덱스로 접근하여 각 문자열 출력
    }

    openHour=atoi(userTime[0]);
    openMin=atoi(userTime[1]);
    closeHour=atoi(userTime[2]);
    closeMin=atoi(userTime[3]);
    
}

int main() {
	int rc, id=12;

    //-- 초기화
    wiringPiSetup();
    pinMode(SWITCH,INPUT); 
    pinMode(LED,OUTPUT);
    digitalWrite(LED, LOW);


	mosquitto_lib_init();

	struct mosquitto *mosq;
    //새로운 클라이언트를 생성한다 
	mosq = mosquitto_new("subscribe-test", true, &id);
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_message_callback_set(mosq, on_message);
	
	rc = mosquitto_connect(mosq, MQTT_HOSTNAME, 1883, 60);
	if(rc) {
		printf("Could not connect to Broker with return code %d\n", rc);
		return -1;
	}

	mosquitto_loop_start(mosq);
	printf("Press Enter to quit...\n");
    for(;;){
        time(&rawtime);     //Return the current time and put it in *TIMER if TIMER is not NULL.
        tm=localtime_r(&rawtime, &newtime); //Return the `struct newtime' representation of *TIMER in local time, using *TP to store the result.
        int sw=0;

        if(digitalRead(SWITCH)==HIGH){          //스위치를 누르면
            sw=1; 
        }

        if (sw==1){
            char set[10] = "T";
            mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(set), set, 0, false);
            
            //printf("Input hour min: ");         //사용자가 시간을 입력할 수 있도록 함.
            //scanf("%d %d",&userHour,&userMin);
            delay(300);
            sw=0;
        }
         if(openHour == (tm->tm_hour) && openMin == tm->tm_min){ 
            for(i=0; i<2000; i++)
				{
			        digitalWrite(LED,HIGH); //LED켜기
				    digitalWrite(STEP_OUT1A,1);
					usleep(2000);
					digitalWrite(STEP_OUT1A,0);
						    
					digitalWrite(STEP_OUT1B,1);
					usleep(2000);
					digitalWrite(STEP_OUT1B,0);
						    
				    digitalWrite(STEP_OUT2A,1);
					usleep(2000);
					digitalWrite(STEP_OUT2A,0);
						    
					digitalWrite(STEP_OUT2B,1);
					usleep(2000);
					digitalWrite(STEP_OUT2B,0);
				}
                    digitalWrite(LED,LOW);  //LED끄기
            }
        if(closeHour == (tm->tm_hour) && closeMin == tm->tm_min){ 
            digitalWrite(STEP_OUT2B,0);
				    usleep(2000);
				    digitalWrite(STEP_OUT2B,1);
				    
				    digitalWrite(STEP_OUT2A,0);
				    usleep(2000);
				    digitalWrite(STEP_OUT2A,1);
				    
				    digitalWrite(STEP_OUT1B,0);
				    usleep(2000);
				    digitalWrite(STEP_OUT1B,1);
				    
				    digitalWrite(STEP_OUT1A,0);
				    usleep(2000);
				    digitalWrite(STEP_OUT1A,1);
        }
    }
	getchar();
	mosquitto_loop_stop(mosq, true);

	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();

	return 0;
}
