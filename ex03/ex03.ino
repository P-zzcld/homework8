#define LED_PIN 4
const unsigned long dotOn = 200;    // 短闪S
const unsigned long dashOn = 600;   // 长闪O
const unsigned long gap = 200;      // 字符间隔熄灭
const unsigned long endPause = 2000;// 整套SOS结束长停顿

// SOS序列：三短、三长、三短 0=短，1=长
int sosSeq[] = {0,0,0, 1,1,1, 0,0,0};
int seqLen = sizeof(sosSeq)/sizeof(int);

unsigned long prevTime = 0;
int curIdx = 0;
int state = 0; // 0熄灭，1点亮

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  unsigned long curTime = millis();
  unsigned long delta = curTime - prevTime;
  unsigned long dur;

  if(state == 1){
    dur = (sosSeq[curIdx]==0) ? dotOn : dashOn;
    if(delta >= dur){
      digitalWrite(LED_PIN, LOW);
      Serial.print("符号");Serial.print(curIdx);Serial.println(" 熄灭");
      state = 0;
      prevTime = curTime;
      curIdx++;
    }
  }else{
    if(curIdx >= seqLen){
      // 整套SOS播放完成，长停顿
      if(delta >= endPause){
        curIdx = 0;
        prevTime = curTime;
        Serial.println("一轮SOS播放完毕，重新开始");
      }
    }else{
      if(delta >= gap){
        digitalWrite(LED_PIN, HIGH);
        state = 1;
        prevTime = curTime;
      }
    }
  }
}