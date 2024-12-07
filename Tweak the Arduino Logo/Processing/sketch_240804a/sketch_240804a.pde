import processing.serial.*;
Serial myPort;

PImage logo;

int bgColor = 0;

void setup() {
  size(640, 640);
  colorMode(HSB, 255);
  
  logo = loadImage("logo.png");
  println(logo.width, logo.height);
  
  println("Available ports:");
  println(Serial.list());
  
  myPort = new Serial(this, Serial.list()[2], 9600);
}

void draw() {
  if(myPort.available() > 0) {
    bgColor = myPort.read();
    println(bgColor);
  }
  
  background(bgColor, 255, 255);
  image(logo, 0, 0);
}
