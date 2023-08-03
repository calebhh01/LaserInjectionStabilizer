int analogPin = A2; // analog input
int outPin = A0; // DAC
int avgnum = 30; // number of times to compute peak max height before averaging
int avgnumtwo = 5;
float scantime = 0.1; // how long to wait before recording each peak height
float conv = 1023/3.3; // digtal to analog ratio
float a = 0.33; // used to convert current to voltage
float b = 1.65; // used to convert current to voltage
float currentjump = 1; //initial current increase in recovery mode
float currentrange = 5; //range of current arduino can tell current controller to do
int res = 10;
float threshb = 0.95;
/* do not change following code */
float sampleArray[1000]; // arrays to hold scanner data
float testArray[1000]; 
float threshold = 0; //variables used to compare peak height
float test = 0;
float amps = 0;
float maxsum;
void setup() {
  Serial.begin(9600);
  pinMode(analogPin,INPUT); // set analog pin mode to input
  analogWriteResolution(res); // set resolution of DAC
  analogWrite(outPin, (a * amps + b) * conv);
  maxsum = 0;
  //Serial.println("acquiring threshold...");
  for (int i = 0; i < avgnum; i++) { // fill array with scanner data
    for (int i = 0; i < 1000; i++) {
      sampleArray[i] = analogRead(analogPin);
      delay(scantime);
    }
    float max_v = sampleArray[0]; // determine max peak height and set as threshold
    for ( int i = 1; i < 1000; i++ ) {
      if ( sampleArray[i] > max_v ) {
        max_v = sampleArray[i];
      }
    }
    maxsum += max_v;
  }
  threshold = threshb * (maxsum/avgnum);
  //Serial.println("threshold acquired:");
  //Serial.println(threshold);
}
void loop() {
  analogWrite(outPin, (a * amps + b) * conv);
  test = maxsum/avgnum; // continuously test new peak data against threshold
  //Serial.println("printing test values:");
  while (test > threshold){
    float maxsumm = 0;
    for (int i = 0; i < avgnum; i++) {
      for (int i = 0; i < 1000; i++) {
        testArray[i] = analogRead(analogPin);
        delay(scantime);
      }
      float max_s = testArray[0];
      for ( int i = 0; i < 1000; i++ ) {
        if ( testArray[i] > max_s ) {
          max_s = testArray[i];
        }
      }
      maxsumm += max_s;
    }
    test =  maxsumm/avgnum;
    //Serial.println(test);
  }
  //Serial.println("lock lost!");
  //Serial.println("relocking...");
  delay(1000);
  while (test < threshold) { //recovery mode
    if (amps > currentrange){ // a safety mechanism to make sure arduino isn't being told to output >5 volts
      amps = (currentrange - 1);
    }
    if (amps < - currentrange){
      amps = 0;
    }
    if (amps < currentrange && amps > - currentrange) { // initial current increase to above lock range
      amps += currentjump;
    }
    analogWrite(outPin, (a * amps + b) * conv);
    //Serial.println(amps);
    while (test < threshold) { // run downwards in current by 0.1 until lock is reestablished
      if (amps > currentrange){ // a safety mechanism to make sure arduino isn't being told to output >5 volts
        amps = (currentrange - 1);
      }
      if (amps < - currentrange){
        amps = 0;
      }
      if (amps < currentrange && amps > - currentrange) {
        amps -= 0.1;
      }
      analogWrite(outPin, (a * amps + b) * conv);
      //Serial.println(amps);
      float maxsumm = 0;
      float minsumm = 0;
      for (int i = 0; i < avgnumtwo; i++) {
        for (int i = 0; i < 1000; i++) {
          testArray[i] = analogRead(analogPin);
          delay(scantime);
        }
        float max_s = testArray[0];
        for ( int i = 0; i < 1000; i++ ) {
          if ( testArray[i] > max_s ) {
            max_s = testArray[i];
          }
        }
        maxsumm += max_s;
      }
      test =  maxsumm/avgnumtwo;
      //Serial.println(test);
    }
  }
  //Serial.println("relocked!");
  //Serial.println("threshold:");
  //Serial.println(threshold);
}
