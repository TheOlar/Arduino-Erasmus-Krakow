#include <LiquidCrystal.h>

#define SPEED_PAC 150
#define SPEED_GHOST 2000

#define MAXX 15
#define MAXY 1

#define btnRight     0
#define btnUp        1
#define btnDown      2
#define btnLeft      3
#define btnSelect    4
#define btnNone      5

void(* resetFunc) (void) = 0;
LiquidCrystal   lcd(8, 9, 4, 5, 6, 7); 


// PACMAN
byte pacman[8] = {
  B00000,
  B10001,
  B11011,
  B01110,
  B11011,
  B11100,
  B01110,
  B00000
};

// GHOST
byte ghost[8] = {
  B00000,
  B00000,
  B01110,
  B10101,
  B11111,
  B11111,
  B10101,
  B00000
};

// POINTS  //INIMIOAREE
byte point[8] = {
  B00000,
  B00000,
  B11011,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

byte   points[MAXX+1][MAXY+1];

int xpac=2; 
int ypac=1;
 
int xghost=15;
int yghost=0;

byte light=true;   
long keystruck=0; 

long pursuit=0; ///ceva pule   
byte gameInProgress=true;
byte vide=false; 

byte level=0; 
int score=0; 

void move_pac(int x,int y)  //move pacman
{
  int oldx=xpac;
  int oldy=ypac;
  
  if (((xpac+x)>=0)&((xpac+x)<=MAXX))   
      xpac=xpac+x; 
  if (((ypac+y)>=0)&((ypac+y)<=MAXY))
      ypac=ypac+y;
  
  lcd.setCursor(xpac,ypac);
  lcd.write(byte(0)); // put pacman 
  lcd.setCursor(oldx,oldy); 
  
  if ((xpac!=oldx)||(ypac!=oldy))
    lcd.print(" "); // when pacman moves we replace the old pos with " "
  
  if(points[xpac][ypac]){
    points[xpac][ypac]=false; // eat
    score++;
  }
  
  vide=true; //empty
  
  for (int i=0; i<=MAXX; i=i+1)
     for (int j=0; j<=MAXY; j=j+1)
        if (points[i][j])  
           vide=false;
 
  if   ((vide)&&(gameInProgress))
      win_game();
}


void lose_game(){
  lcd.setCursor(0,   0); 
  lcd.print("**Game Over*"); 
  lcd.setCursor(0, 1); 
  lcd.print("*");
   lcd.print(score);
  lcd.print("*             "); 
  delay(2000);
  resetFunc();
}

void win_game()
{
  level++;
  lcd.setCursor(0, 0); 
  lcd.print("*Next level*"); 
  lcd.setCursor(0, 1);
  lcd.print("*                 ");
  lcd.print(level,DEC);
   lcd.print(" *"); 
  delay(2000);  
  initLevel(); 
}

void continue_game() //move ghost
{
  int oldx=xghost;
  int oldy=yghost;
  
   if (yghost<ypac) 
      yghost=yghost+1;
  else if 
      (yghost>ypac) yghost=yghost-1;
  else if 
      (xghost<xpac) xghost=xghost+1;
  else if 
      (xghost>xpac) xghost=xghost-1;
  
  lcd.setCursor(xghost,yghost);  
  lcd.write(1); 
  
  lcd.setCursor(oldx,oldy); 
  
  if ((oldx!=xghost)||(oldy!=yghost)) 
  {
     if (points[oldx][oldy]) 
         lcd.write(2); 
     else lcd.print(" "); 
  }

  
}


void initLevel(){
  for (int i=0; i<=MAXX; i=i+1)
    for (int   j=0; j<=MAXY; j=j+1){
      points[i][j]=true; 
      lcd.setCursor(i-1, j-1); 
      lcd.write(2); 
  }

  
  lcd.setCursor(xpac,ypac);   
  lcd.write(byte(0)); 
  
  lcd.setCursor(xghost,yghost); 
  lcd.write(1); 
  
  pursuit=millis(); //so it wont move right away
  vide=false;
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.createChar(0, pacman); 
  lcd.createChar(1, ghost);   
  lcd.createChar(2, point); 
  lcd.setCursor(0, 0); 
  lcd.print("Ms.Pacman! UwU"); 
  
  delay (5000); 
  initLevel(); 
}

void loop() {
  int thisChar = Serial.read();
  
  switch   (thisChar)
  {
  case 'r':
    lcd.scrollDisplayRight();
    break;
   case 'l':
    lcd.scrollDisplayLeft();
    break;
  }
  
  if ((thisChar>'a')&(thisChar<'z')){
    lcd.setCursor(1,1);
    lcd.write(thisChar);
  }
  
  if (millis()-keystruck>SPEED_PAC){
    int joy=getKey();
     switch (joy)
    {
    case btnNone:
      break;
      
    case btnLeft:
      move_pac(-1,0);
       keystruck=millis(); // reset movement timer
      break;
      
     case btnRight:
      move_pac(1,0);
      keystruck=millis();   // reset movement timer
      break;
      
    case btnUp:
      move_pac(0,-1);
      keystruck=millis(); // reset movement timer
       break;
       
    case btnDown:
      move_pac(0,1);
      keystruck=millis(); // reset movement timer
      break;
      
    default:
      Serial.print(joy); 
      keystruck=millis();   // reset movement timer
    }; 
  };
  
  if (millis()-pursuit>SPEED_GHOST/(level+1)+10)
  {
    continue_game();
    pursuit=millis();
  }
  
  if ((xpac==xghost)&&(ypac==yghost)&&(gameInProgress))   
  {
    lose_game();
  }
}


int getKey() {
  int b = analogRead(A0);
   if (b > 1000) return btnNone;
  delay(8);
  if (b < 50)  return btnRight;
  if (b < 180) return btnUp;
  if (b < 330) return btnDown;
  if (b < 520) return btnLeft;
  if (b < 700) return btnSelect;
}
