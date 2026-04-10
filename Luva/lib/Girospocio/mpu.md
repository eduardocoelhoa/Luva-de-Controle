# Documentação Completa da Implementação do MPU6050

## Índice
1. [Introdução ao MPU6050](#introdução-ao-mpu6050)
2. [Comunicação I2C](#comunicação-i2c)
3. [Endereços e Registradores](#endereços-e-registradores)
4. [Estrutura da Classe MPU6050](#estrutura-da-classe-mpu6050)
5. [Processo de Inicialização](#processo-de-inicialização)
6. [Leitura e Processamento de Dados](#leitura-e-processamento-de-dados)
7. [Calibração do Giroscópio](#calibração-do-giroscópio)
8. [Filtro Complementar](#filtro-complementar)

---

## Introdução ao MPU6050

O **MPU6050** é um sensor inercial (IMU - Inertial Measurement Unit) que combina dois sensores em um único chip:
- **Acelerômetro de 3 eixos**: Mede a aceleração linear nos eixos X, Y e Z
- **Giroscópio de 3 eixos**: Mede a velocidade angular (rotação) nos eixos X, Y e Z

Além disso, o sensor também possui um sensor de temperatura embutido.

### Para que serve?

O MPU6050 é amplamente usado para detectar movimento, orientação e inclinação de objetos. Neste projeto da luva de controle, ele permite:
- Detectar a rotação da mão (roll, pitch, yaw)
- Capturar movimentos rápidos
- Calcular ângulos de inclinação

---

## Comunicação I2C

### O que é I2C?

**I2C** (Inter-Integrated Circuit) é um protocolo de comunicação serial que permite a troca de dados entre um microcontrolador (como o ESP32) e dispositivos periféricos (como o MPU6050) usando apenas **2 fios**:

- **SDA (Serial Data)**: Linha de dados bidirecional
- **SCL (Serial Clock)**: Linha de clock controlada pelo mestre (microcontrolador)

### Como funciona a comunicação?

1. **Endereçamento**: Cada dispositivo I2C tem um endereço único. O MPU6050 usa o endereço **0x68** por padrão
2. **Mestre-Escravo**: O microcontrolador (mestre) inicia todas as comunicações. O MPU6050 (escravo) responde quando solicitado
3. **Transações**:
   - **Escrita**: O mestre envia o endereço do registrador e o dado a ser escrito
   - **Leitura**: O mestre solicita dados de um registrador específico

### Implementação no código

Na nossa classe, usamos a biblioteca `Wire` do Arduino para comunicação I2C:

```cpp
TwoWire *wire;  // Ponteiro para interface I2C (permite usar Wire ou Wire1)
```

#### Escrita em um Registrador

```cpp
void MPU6050::writeRegister(byte reg, byte data) {
  wire->beginTransmission(MPU6050_ADDR);  // Inicia transmissão para o MPU6050
  wire->write(reg);                        // Envia o endereço do registrador
  wire->write(data);                       // Envia o valor a ser escrito
  wire->endTransmission();                 // Finaliza a transmissão
}
```

**Passo a passo:**
1. `beginTransmission()`: Avisa ao barramento I2C que queremos falar com o dispositivo no endereço 0x68
2. `write(reg)`: Especifica qual registrador do MPU6050 queremos modificar
3. `write(data)`: Envia o valor que queremos colocar naquele registrador
4. `endTransmission()`: Encerra a comunicação

#### Leitura de um Registrador

```cpp
byte MPU6050::readRegister(byte reg) {
  wire->beginTransmission(MPU6050_ADDR);   // Inicia transmissão
  wire->write(reg);                         // Diz qual registrador queremos ler
  wire->endTransmission(true);              // Finaliza (true = envia STOP)
  wire->requestFrom(MPU6050_ADDR, 1);       // Solicita 1 byte de dados
  return wire->read();                      // Lê e retorna o byte recebido
}
```

**Passo a passo:**
1. Primeiro, dizemos ao MPU6050 qual registrador queremos ler
2. Finalizamos essa "escrita" com um sinal de STOP
3. Pedimos ao MPU6050 para enviar 1 byte de dados
4. Lemos esse byte e retornamos

---

## Endereços e Registradores

Os registradores são posições de memória dentro do MPU6050 onde podemos ler ou escrever configurações e dados. Cada registrador tem um endereço hexadecimal específico.

### Registradores Utilizados

```cpp
#define MPU6050_ADDR         0x68  // Endereço I2C do sensor
#define MPU6050_SMPLRT_DIV   0x19  // Taxa de amostragem
#define MPU6050_CONFIG       0x1A  // Configuração geral
#define MPU6050_GYRO_CONFIG  0x1B  // Configuração do giroscópio
#define MPU6050_ACCEL_CONFIG 0x1C  // Configuração do acelerômetro
#define MPU6050_PWR_MGMT_1   0x6B  // Gerenciamento de energia
```

### Detalhamento dos Registradores

#### 1. MPU6050_ADDR (0x68)
- **Não é um registrador**, mas sim o **endereço I2C** do dispositivo
- Usado em toda comunicação para identificar o sensor no barramento

#### 2. SMPLRT_DIV (0x19) - Sample Rate Divider
- Controla a **taxa de amostragem** dos sensores
- Fórmula: `Taxa de Amostragem = Taxa do Giroscópio / (1 + SMPLRT_DIV)`
- **Valor usado: 0x00** → Taxa máxima (1kHz se o filtro estiver desabilitado)

#### 3. CONFIG (0x1A) - Configuração Geral
- Configura o **filtro passa-baixa digital** (DLPF)
- **Valor usado: 0x00** → Filtro desativado, banda passante máxima

#### 4. GYRO_CONFIG (0x1B) - Configuração do Giroscópio
- Define a **escala de medição** do giroscópio
- **Valor usado: 0x08** → Escala de ±500 °/s (graus por segundo)
- Outras opções: ±250, ±1000, ±2000 °/s
- **Fator de conversão**: Para ±500 °/s, dividimos o valor bruto por **65.5** para obter °/s

#### 5. ACCEL_CONFIG (0x1C) - Configuração do Acelerômetro
- Define a **escala de medição** do acelerômetro
- **Valor usado: 0x00** → Escala de ±2g (onde g = 9.8 m/s²)
- Outras opções: ±4g, ±8g, ±16g
- **Fator de conversão**: Para ±2g, dividimos o valor bruto por **16384** para obter g

#### 6. PWR_MGMT_1 (0x6B) - Gerenciamento de Energia
- Controla o modo de energia e a fonte de clock
- **Valor usado: 0x01** → Usa o clock do giroscópio X (mais estável que o oscilador interno)
- Valor 0x00 tiraria o sensor do modo sleep (inativo)

### Registradores de Dados (Leitura)

Os dados dos sensores ficam armazenados em registradores sequenciais:

**Início dos dados: 0x3B**
```
0x3B-0x3C: ACCEL_XOUT (16 bits) - Aceleração X
0x3D-0x3E: ACCEL_YOUT (16 bits) - Aceleração Y
0x3F-0x40: ACCEL_ZOUT (16 bits) - Aceleração Z
0x41-0x42: TEMP_OUT   (16 bits) - Temperatura
0x43-0x44: GYRO_XOUT  (16 bits) - Giroscópio X
0x45-0x46: GYRO_YOUT  (16 bits) - Giroscópio Y
0x47-0x48: GYRO_ZOUT  (16 bits) - Giroscópio Z
```

Como os registradores são sequenciais, podemos **ler todos de uma vez** em uma única transação I2C (14 bytes no total), o que é muito mais eficiente!

---

## Estrutura da Classe MPU6050

A classe `MPU6050` segue os princípios de **Programação Orientada a Objetos (POO)**, encapsulando toda a lógica de comunicação e processamento do sensor.

### Princípios de POO Aplicados

#### 1. **Encapsulamento**
- Dados privados (`private`) são protegidos e só podem ser acessados por métodos públicos (`public`)
- Isso evita modificações acidentais e garante que os dados sejam sempre acessados de forma controlada

```cpp
private:
    int16_t rawAccX, rawAccY, rawAccZ;    // Usuário não pode modificar diretamente
    
public:
    float getAccX() const { return accX; } // Acesso controlado via getter
```

#### 2. **Abstração**
- A complexidade da comunicação I2C e dos cálculos é escondida do usuário
- O usuário simplesmente chama `update()` e `getAngleX()` sem precisar entender os detalhes internos

#### 3. **Modularização**
- Cada método tem uma responsabilidade clara
- `begin()` → Inicialização
- `update()` → Leitura e processamento
- `calibrate()` → Calibração

### Membros da Classe

#### Atributos Privados

```cpp
private:
    TwoWire *wire;              // Ponteiro para interface I2C
    float accCoef, gyroCoef;    // Coeficientes do filtro complementar
    
    // Dados brutos (16 bits com sinal)
    int16_t rawAccX, rawAccY, rawAccZ;
    int16_t rawGyroX, rawGyroY, rawGyroZ;
    int16_t rawTemp;
    
    // Offsets de calibração
    float gyroXoffset, gyroYoffset, gyroZoffset;
    
    // Dados convertidos (em unidades físicas)
    float accX, accY, accZ;           // Em 'g' (gravidade)
    float gyroX, gyroY, gyroZ;        // Em °/s
    float temperature;                 // Em °C
    
    // Ângulos calculados
    float angleAccX, angleAccY;       // Ângulos do acelerômetro
    float angleX, angleY, angleZ;     // Ângulos finais (fusão sensorial)
    
    // Controle de tempo
    long preInterval;                  // Para calcular delta-time
```

**Por que int16_t?**
- Os dados do MPU6050 são de 16 bits com sinal (valores de -32768 a +32767)
- `int16_t` garante que sempre teremos exatamente 16 bits, independentemente da plataforma

#### Métodos Públicos

```cpp
public:
    MPU6050(TwoWire &w, float accCoef = 0.02f, float gyroCoef = 0.98f);
    void begin();                     // Inicializa o sensor
    void update();                    // Atualiza todos os dados
    void calibrate(bool consoleOutput = true);  // Calibra o giroscópio
    
    // Getters inline (definidos no .h para otimização)
    float getAngleX() const { return angleX; }  // Roll
    float getAngleY() const { return angleY; }  // Pitch
    float getAngleZ() const { return angleZ; }  // Yaw
    
    float getAccX() const { return accX; }
    float getAccY() const { return accY; }
    float getAccZ() const { return accZ; }
    
    float getGyroX() const { return gyroX; }
    float getGyroY() const { return gyroY; }
    float getGyroZ() const { return gyroZ; }
    
    float getTemp() const { return temperature; }
```

**Modificador `const`:**
- Indica que o método não modifica o estado do objeto
- Permite que métodos getters sejam chamados em objetos constantes
- Boa prática de programação para indicar funções "somente leitura"

#### Métodos Privados

```cpp
private:
    void writeRegister(byte reg, byte data);
    byte readRegister(byte reg);
```

Esses métodos são privados porque são **detalhes de implementação** que o usuário não precisa (e não deve) acessar diretamente.

### Construtor

```cpp
MPU6050::MPU6050(TwoWire &w, float aC, float gC) {
  wire = &w;              // Armazena ponteiro para interface I2C
  accCoef = aC;           // Coeficiente do acelerômetro (padrão 0.02)
  gyroCoef = gC;          // Coeficiente do giroscópio (padrão 0.98)
  gyroXoffset = 0; 
  gyroYoffset = 0; 
  gyroZoffset = 0;
  angleX = 0; 
  angleY = 0; 
  angleZ = 0;
}
```

**Parâmetros:**
- `TwoWire &w`: Referência à interface I2C (permite usar Wire, Wire1, etc.)
- `float aC, gC`: Coeficientes do filtro complementar com valores padrão

**Inicialização:**
- Configura ponteiros e variáveis iniciais
- Os offsets começam em zero (serão ajustados na calibração)

---

## Processo de Inicialização

O método `begin()` é chamado no setup do programa para configurar o sensor.

```cpp
void MPU6050::begin(){
  // Configuração dos registradores
  writeRegister(MPU6050_SMPLRT_DIV, 0x00);      // Taxa de amostragem máxima
  writeRegister(MPU6050_CONFIG, 0x00);          // Filtro passa-baixa desativado
  writeRegister(MPU6050_GYRO_CONFIG, 0x08);     // Escala de +/- 500 dps
  writeRegister(MPU6050_ACCEL_CONFIG, 0x00);    // Escala de +/- 2g
  writeRegister(MPU6050_PWR_MGMT_1, 0x01);      // Usa o clock do oscilador interno
  
  // Inicialização dos ângulos baseada apenas no acelerômetro
  this->update();
  angleX = angleAccX;
  angleY = angleAccY;
  
  preInterval = millis();  // Marca o tempo inicial
}
```

### Passo a Passo

1. **Configuração dos Registradores**: Cada `writeRegister()` configura um aspecto do sensor
2. **Leitura Inicial**: `update()` lê os dados uma vez
3. **Inicialização dos Ângulos**: Os ângulos iniciais são baseados no acelerômetro (referência gravitacional)
4. **Marca do Tempo**: `preInterval` guarda o tempo em milissegundos para calcular o delta-time posteriormente

### Por que inicializar com o acelerômetro?

- O acelerômetro detecta a gravidade, fornecendo uma referência de orientação absoluta
- O giroscópio mede apenas **mudanças** de ângulo, não a posição absoluta
- Começar com o acelerômetro garante que temos uma orientação inicial correta

---

## Leitura e Processamento de Dados

O método `update()` é o coração da classe. Ele lê todos os dados do sensor e calcula os ângulos de orientação.

### Etapa 1: Leitura dos Dados Brutos

```cpp
void MPU6050::update() {
  wire->beginTransmission(MPU6050_ADDR);
  wire->write(0x3B);  // Registrador inicial (Accel X)
  wire->endTransmission(false);  // false = mantém conexão ativa
  wire->requestFrom((int)MPU6050_ADDR, 14);  // Solicita 14 bytes
  
  // Lê os 14 bytes sequencialmente
  rawAccX = wire->read() << 8 | wire->read();
  rawAccY = wire->read() << 8 | wire->read();
  rawAccZ = wire->read() << 8 | wire->read();
  rawTemp = wire->read() << 8 | wire->read();
  rawGyroX = wire->read() << 8 | wire->read();
  rawGyroY = wire->read() << 8 | wire->read();
  rawGyroZ = wire->read() << 8 | wire->read();
```

**Operação de bit-shift:**
```cpp
rawAccX = wire->read() << 8 | wire->read();
```
- O MPU6050 envia cada valor de 16 bits em 2 bytes (big-endian)
- Primeiro byte (MSB - Most Significant Byte): bits mais significativos
- Segundo byte (LSB - Least Significant Byte): bits menos significativos
- `<< 8`: Desloca o MSB 8 bits à esquerda
- `|`: Combina (OR) com o LSB para formar o valor de 16 bits completo

**Exemplo numérico:**
```
MSB = 0x1A (26 em decimal) = 00011010
LSB = 0x2F (47 em decimal) = 00101111

MSB << 8 = 0x1A00 = 0001101000000000
MSB << 8 | LSB = 0x1A2F = 0001101000101111 (6703 em decimal)
```

### Etapa 2: Conversão para Unidades Físicas

```cpp
  // Temperatura
  temperature = (rawTemp + 12412.0) / 340.0;
  
  // Acelerômetro (±2g)
  accX = ((float)rawAccX) / 16384.0;
  accY = ((float)rawAccY) / 16384.0;
  accZ = ((float)rawAccZ) / 16384.0;
  
  // Giroscópio (±500 °/s)
  gyroX = ((float)rawGyroX) / 65.5;
  gyroY = ((float)rawGyroY) / 65.5;
  gyroZ = ((float)rawGyroZ) / 65.5;
```

**Fatores de conversão:**
- **Temperatura**: Fórmula do datasheet do MPU6050
- **Acelerômetro (±2g)**: Sensibilidade = 16384 LSB/g (do datasheet)
- **Giroscópio (±500°/s)**: Sensibilidade = 65.5 LSB/(°/s) (do datasheet)

Esses valores convertem os dados brutos em unidades que podemos entender (g, °/s, °C).

### Etapa 3: Aplicação dos Offsets de Calibração

```cpp
  gyroX -= gyroXoffset;
  gyroY -= gyroYoffset;
  gyroZ -= gyroZoffset;
```

Os offsets removem o **erro sistemático** (drift) do giroscópio. Eles são calculados durante a calibração.

### Etapa 4: Cálculo dos Ângulos do Acelerômetro

```cpp
  angleAccX = atan2(accY, accZ) * 180 / PI;      // Roll
  angleAccY = atan2(-accX, accZ) * 180 / PI;     // Pitch
```

**O que é atan2?**
- Função que calcula o ângulo de um ponto (y, x) em relação ao eixo X
- Retorna valores em **radianos** de -π a +π
- Multiplicamos por `180/PI` para converter em **graus**

**Por que esses eixos?**
- O acelerômetro mede a aceleração da gravidade (que sempre "aponta para baixo")
- Roll (rotação lateral): Comparamos Y e Z
- Pitch (inclinação frente/trás): Comparamos -X e Z
- Yaw (rotação horizontal): **Não pode** ser calculado com acelerômetro (não há referência gravitacional)

### Etapa 5: Cálculo do Delta-Time

```cpp
  unsigned long currentMillis = millis();
  float interval = (currentMillis - preInterval) * 0.001;  // Converte ms para s
  preInterval = currentMillis;
```

O **delta-time** (intervalo de tempo) é necessário para integrar a velocidade angular do giroscópio e obter ângulos.

### Etapa 6: Integração do Giroscópio

```cpp
  angleX += gyroX * interval;
  angleY += gyroY * interval;
  angleZ += gyroZ * interval;
```

Como o giroscópio mede **velocidade angular** (°/s), precisamos multiplicar pelo tempo para obter a **mudança de ângulo**:
```
Δângulo = velocidade_angular × Δtempo
```

### Etapa 7: Filtro Complementar (Fusão Sensorial)

```cpp
  angleX = (gyroCoef * (angleX + gyroX * interval)) + (accCoef * angleAccX);
  angleY = (gyroCoef * (angleY + gyroY * interval)) + (accCoef * angleAccY);
  // angleZ usa apenas gyro (sem correção do acelerômetro)
```

Este é o passo mais importante! Veja a seção [Filtro Complementar](#filtro-complementar) para detalhes.

---

## Calibração do Giroscópio

### Por que calibrar?

O giroscópio possui um **drift** (erro sistemático) que faz com que, mesmo parado, ele reporte pequenos valores de rotação. Com o tempo, isso acumula erro nos ângulos calculados.

### Como funciona a calibração?

```cpp
void MPU6050::calibrate(bool consoleOutput) {
  float x = 0, y = 0, z = 0;
  int16_t rx, ry, rz;
  
  if (consoleOutput) {
    Serial.println(F("Calibrando Giroscopio... Mantenha a luva parada."));
  }
  
  delay(1000);  // Pausa para estabilização
  
  int numSamples = 2000;
  for (int i = 0; i < numSamples; i++) {
    // Lê dados do giroscópio (registrador 0x43)
    wire->beginTransmission(MPU6050_ADDR);
    wire->write(0x43);
    wire->endTransmission(false);
    wire->requestFrom((int)MPU6050_ADDR, 6);
    
    rx = wire->read() << 8 | wire->read();
    ry = wire->read() << 8 | wire->read();
    rz = wire->read() << 8 | wire->read();
    
    // Converte e acumula
    x += ((float)rx) / 65.5;
    y += ((float)ry) / 65.5;
    z += ((float)rz) / 65.5;
  }
  
  // Calcula a média
  gyroXoffset = x / numSamples;
  gyroYoffset = y / numSamples;
  gyroZoffset = z / numSamples;
  
  if (consoleOutput) {
    Serial.println(F("Calibracao Concluida!"));
  }
}
```

### Processo de Calibração

1. **Mantém o sensor imóvel**: O usuário deve deixar a luva completamente parada
2. **Coleta 2000 amostras**: Lê o giroscópio 2000 vezes
3. **Calcula a média**: A média representa o erro sistemático
4. **Armazena os offsets**: Esses valores serão subtraídos de todas as leituras futuras

**Por que 2000 amostras?**
- Mais amostras = média mais precisa
- Remove ruído aleatório
- 2000 é um bom equilíbrio entre precisão e tempo de calibração

---

## Filtro Complementar

### O Problema

Cada sensor tem **vantagens e desvantagens**:

| Sensor | Vantagens | Desvantagens |
|--------|-----------|--------------|
| **Acelerômetro** | Referência absoluta (gravidade), sem drift | Sensível a vibrações e acelerações lineares |
| **Giroscópio** | Resposta rápida, preciso no curto prazo | Acumula erro ao longo do tempo (drift) |

Se usarmos apenas um sensor:
- **Só acelerômetro**: Ângulos oscilam com vibrações
- **Só giroscópio**: Ângulos "flutuam" com o tempo (drift)

### A Solução: Filtro Complementar

O filtro complementar **combina os dois sensores** aproveitando os pontos fortes de cada um:

```cpp
angleX = (gyroCoef * (angleX + gyroX * interval)) + (accCoef * angleAccX);
```

**Estrutura:**
```
ângulo_final = (peso_gyro × ângulo_gyro) + (peso_acc × ângulo_acc)
```

**Valores típicos:**
- `gyroCoef = 0.98` (98% do giroscópio)
- `accCoef = 0.02` (2% do acelerômetro)
- `gyroCoef + accCoef = 1.0` (100%)

### Como Funciona?

1. **Curto prazo (movimentos rápidos)**:
   - O giroscópio (98%) domina
   - Resposta rápida e suave aos movimentos

2. **Longo prazo (posição estável)**:
   - O acelerômetro (2%) gradualmente corrige o drift
   - Previne que o ângulo "flutue"

### Analogia

Imagine que você está dirigindo um carro:
- **Giroscópio = Volante**: Responde imediatamente às suas ações, mas pode "desviar" aos poucos
- **Acelerômetro = GPS**: Sabe onde você realmente está, mas atualiza devagar e pode ter erros momentâneos
- **Filtro Complementar**: Você usa principalmente o volante para dirigir, mas de vez em quando olha o GPS para garantir que está no caminho certo

### Visualização Matemática

```
Frame 1: angleX = 0.98 × (angleX_anterior + Δgyro) + 0.02 × angleAcc
         ↑                 ↑                           ↑
      resultado       integração gyro          correção acc
```

A cada atualização:
- Pegamos 98% do ângulo calculado pelo giroscópio
- Adicionamos 2% do ângulo medido pelo acelerômetro
- Isso faz uma **correção lenta mas contínua** do drift

### Por que não usar Kalman Filter?

O Filtro de Kalman é mais preciso, mas:
- Muito mais complexo de implementar
- Requer mais processamento
- O Filtro Complementar é **"suficientemente bom"** para a maioria das aplicações
- Simples de entender e ajustar

---

## Resumo do Fluxo de Dados

```
┌──────────────────────────────────────────────────────────┐
│ 1. Leitura I2C                                           │
│    MPU6050 → 14 bytes via I2C → ESP32                    │
└──────────────────────────────────────────────────────────┘
                           ↓
┌──────────────────────────────────────────────────────────┐
│ 2. Conversão de Dados Brutos                             │
│    int16_t → float (unidades físicas: g, °/s, °C)        │
└──────────────────────────────────────────────────────────┘
                            ↓
┌──────────────────────────────────────────────────────────┐
│ 3. Calibração                                            │
│    gyroX -= gyroXoffset (remove drift)                   │
└──────────────────────────────────────────────────────────┘
                           ↓
┌──────────────────────────────────────────────────────────┐
│ 4. Cálculo de Ângulos                                    │
│    • Acelerômetro: atan2() → angleAccX, angleAccY        │
│    • Giroscópio: integração → angleX, angleY, angleZ     │
└──────────────────────────────────────────────────────────┘
                            ↓
┌──────────────────────────────────────────────────────────┐
│ 5. Filtro Complementar (Fusão Sensorial)                 │
│    angleX = 0.98×gyro + 0.02×acc → Ângulo Final          │
└──────────────────────────────────────────────────────────┘
                            ↓
┌──────────────────────────────────────────────────────────┐
│ 6. Disponível via Getters                                │
│    getAngleX(), getAngleY(), getAngleZ()                 │
└──────────────────────────────────────────────────────────┘
```

---

## Exemplo de Uso

```cpp
#include "mpu.h"

MPU6050 mpu(Wire);  // Usa interface I2C padrão

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  mpu.begin();
  mpu.calibrate();  // Mantenha a luva parada durante a calibração
}

void loop() {
  mpu.update();  // Atualiza todos os dados
  
  // Lê os ângulos de orientação
  float roll = mpu.getAngleX();
  float pitch = mpu.getAngleY();
  float yaw = mpu.getAngleZ();
  
  Serial.print("Roll: "); Serial.print(roll);
  Serial.print(" | Pitch: "); Serial.print(pitch);
  Serial.print(" | Yaw: "); Serial.println(yaw);
  
  delay(10);
}
```

---

## Glossário de Termos

- **I2C**: Protocolo de comunicação serial de 2 fios
- **Registrador**: Posição de memória dentro de um chip
- **LSB**: Least Significant Bit/Byte (bit/byte menos significativo)
- **MSB**: Most Significant Bit/Byte (bit/byte mais significativo)
- **Drift**: Erro acumulativo ao longo do tempo
- **Roll**: Rotação lateral (inclinar para os lados)
- **Pitch**: Inclinação frente/trás (cabeça para cima/baixo)
- **Yaw**: Rotação horizontal (girar a cabeça)
- **Fusão Sensorial**: Combinar dados de múltiplos sensores
- **Delta-time**: Intervalo de tempo entre duas medições

---

## Referências

- [Datasheet MPU6050](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf)
- [Register Map MPU6050](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf)
- Protocolo I2C: [I2C Bus Specification](https://www.nxp.com/docs/en/user-guide/UM10204.pdf)
- Filtro Complementar: [Complementary Filter Design](http://www.olliw.eu/2013/imu-data-fusing/)

---

*Última atualização: 12 de Dezembro de 2025*