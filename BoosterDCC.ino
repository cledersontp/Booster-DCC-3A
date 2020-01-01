//
// BOOSTER DCC 3A - Versao 1.0
// By Clederson T. Przybysz - clederson_p@yahoo.com.br
// expressoarduino.blogspot.com
// Criação: Setembro/2019
//
//Copyright Notes Interface 48 Controles:
//O SOFTWARE É FORNECIDO "NO ESTADO EM QUE SE ENCONTRAM", SEM GARANTIA DE QUALQUER TIPO, EXPRESSA OU IMPLÍCITA, MAS NÃO SE LIMITANDO ÀS GARANTIAS DE COMERCIALIZAÇÃO.  
//EM NENHUMA CIRCUNSTÂNCIA, O AUTOR/TITULAR DE DIREITOS AUTORAIS SE RESPONSABILIZA POR QUALQUER RECLAMAÇÃO, DANOS OU OUTRA RESPONSABILIDADE, 
//SEJA EM AÇÃO DE CONTRATO, DELITO OU DE OUTRA FORMA, DECORRENDO DESTE SOFTWARE OU RELACIONADO AO SEU  USO.
//
//Copyright Notes CRMI Library AND AutoRS485 Library: 
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
//OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
#define pinLedOn  6
#define pinLedDcc  5
#define pinLedAtivo  4
#define pinLedCorrente 3
#define pinEnable 13
#define pinSensorDCC A0
#define pinSensorCorrente A1

#define AmonstragemCorrente 0.2
#define LimiteCorrente  46 //(509 = 3A LMD18200T RCS 2k2)

unsigned long TimerCorrente;
unsigned long TimerDCC;

int CorrenteAtual=0;
byte SinalDCC=0;
byte SobreCorrente=0;
byte nErroDcc=0;

void setup() {
  Serial.begin(19200);
  Serial.println("Start");
  //Pinos Saidas
  pinMode(pinLedOn, OUTPUT);
  pinMode(pinLedDcc, OUTPUT);
  pinMode(pinLedAtivo, OUTPUT);
  pinMode(pinLedCorrente, OUTPUT);
  pinMode(pinEnable, OUTPUT);
  //Pinos de Entradas
  pinMode(pinSensorDCC, INPUT);

  //Desliga Shield
  digitalWrite(pinEnable,LOW);
  nErroDcc=5;
  //Leds Off
  digitalWrite(pinLedDcc,LOW);
  digitalWrite(pinLedAtivo,LOW);
  digitalWrite(pinLedCorrente,LOW);
  SinalDCC=0;
  SobreCorrente=0;

  //Led On
  digitalWrite(pinLedOn,HIGH);
}

void loop() {
  byte val=0;
  byte vPulso=0;
  int nPulsos=0;
  
  //Verifica Sinal DCC
  TimerDCC=millis();
  do {
    val=digitalRead(pinSensorDCC);
    if (val != vPulso) {
      vPulso = val;
      if (vPulso==1) nPulsos++; 
    }
  } while (millis()-TimerDCC<2);
  if (nPulsos<10) {
    nErroDcc++;
    //Serial.print("E:");
    //Serial.print(nErroDcc);
    //Serial.print(" P:");
    //Serial.println(nPulsos);
    if (nErroDcc>3&&SinalDCC==1) {
      //Desliga Led DCC
      digitalWrite(pinLedDcc,LOW);
      //Desliga Shield
      digitalWrite(pinEnable,LOW);
      digitalWrite(pinLedAtivo,LOW);
      SinalDCC=0;    
      Serial.println("DCC Off");
    }
  }
  else
  {
    nErroDcc=0;
    if (SinalDCC==0) {
      SinalDCC=1;
      //Liga Led DCC
      digitalWrite(pinLedDcc,HIGH);
      Serial.println("DCC On");
      if (SobreCorrente==0) {
        //Liga Shield
        digitalWrite(pinEnable,HIGH);
        digitalWrite(pinLedAtivo,HIGH);
      }
    }
  }
    
  if (SobreCorrente==1||SinalDCC==0) return;
  //Monitor Corrente Saida
  if (millis()-TimerCorrente>10) {
    CorrenteAtual=analogRead(pinSensorCorrente)*AmonstragemCorrente+CorrenteAtual*(1.0-AmonstragemCorrente);
    //CorrenteAtual=0;
    //Serial.print("c:");
    //Serial.println(CorrenteAtual);
    TimerCorrente=millis();
    
    //Verifica Se Ultrapassou Limite
    if(CorrenteAtual>LimiteCorrente){  
      Serial.print("c:");
      Serial.println(CorrenteAtual); 
      //Desliga Shield
      digitalWrite(pinEnable,LOW);
      digitalWrite(pinLedAtivo,LOW);
      SobreCorrente=1;
      //Led Corrente Off
      digitalWrite(pinLedCorrente,HIGH);
      Serial.println("Desligado - Sobre Corrente");
    } 
  }
}
