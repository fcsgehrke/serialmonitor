# serialmonitor
Monitor de Portas Seriais para Windows

![screenshot](https://raw.githubusercontent.com/fcsgehrke/serialmonitor/master/img/screenshot.png)

# Protocolo:

O protocolo � bem simples. A estrutura � composta dos bytes abaixo. Os frames tem tamanho fixo de 24 bytes. A pergunta e a resposta tem a mesma estrutura. O checksum � calculado iniciando no segundo byte e � bem simples, apenas um soma de todos os bytes.

## Estrutura:

Nome | Byte | Tamanho
---- | ---- |--------
STX | <0x01> | 01
FUN��O | <0x02> | 01
DADOS | <....> | 20
CKS | <....> | 01
ETX | <0x03> | 01


## Fun��es:

Fun��o | Byte
------- | ----
Ler I/O | <0x01>
Escrever I/O | <0x02>
Escrever LCD | <0x03>
Limpar LCD | <0x04>
Escrever PWM | <0x05>

### Ler I/O
#### Envio:
STX | FUNC | Bytes n�o utilizados no envio (devem ser enviados mesmo assim) | CKS | ETX
--- | ---- |--------------------- | --- | ---
0x01 | 0x01 | 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 | 0xC0 | 0x03

#### Resposta:
STX | FUNC | O1 | O2 | O3 | O4 | I1 | I2 | I3 | I4 | PWMC | PWMD | PWMU | Bytes n�o utilizados | CKS | ETX
--- | ---- | -- | -- | -- | -- | -- | -- | -- | -- | ---- | ---- | ---- | ----- | --- | ---
0x01 | 0x01 | 0x31 | 0x30 | 0x31 | 0x30 | 0x30 | 0x30 | 0x30 | 0x30 | 0x30 | 0x30 | 0x30 | 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 | 0xC0 | 0x03


Indicador | Descri��o
--------- | ---------
STX | Byte <0x01> de Inicio de Frame
FUNC | Fun��o
O1 | Valor da saida 1 ('0', '1')
O2 | Valor da saida 2 ('0', '1')
O3 | Valor da saida 3 ('0', '1')
O4 | Valor da saida 4 ('0', '1')
I1 | Valor da entrada 1 ('0', '1')
I2 | Valor da entrada 2 ('0', '1')
I3 | Valor da entrada 3 ('0', '1')
I4 | Valor da entrada 4 ('0', '1')
PWMC | Centena do PWM
PWMD | Dezena do PWM
PWMU | Unidade do PWM
CKS | Checksum
ETX | Byte <0x03> de Fim de Frame


### Escrever I/O
#### Envio:
STX | FUNC | O1 | O2 | O3 | O4 |                                      Bytes n�o utilizados                                  | CKS | ETX
--- | ---- | -- | -- | -- | -- | -------------------------------------------------------------------- | ---- | ---- | ----- | --- | ---
0x01 | 0x02 | 0x31 | 0x30 | 0x31 | 0x30 | 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 | 0xC0 | 0x03

#### Resposta:
N�o h�.

Indicador | Descri��o
--------- | ---------
STX | Byte <0x01> de Inicio de Frame
FUNC | Fun��o
O1 | Valor da saida 1 ('0', '1')
O2 | Valor da saida 2 ('0', '1')
O3 | Valor da saida 3 ('0', '1')
O4 | Valor da saida 4 ('0', '1')
CKS | Checksum
ETX | Byte <0x03> de Fim de Frame


### Escrever LCD
#### Envio:
 STX | FUNC |     X     | Y         | TEXTO                                                                           | CKS  | ETX
---- | ---- | --------- | --------- | ------------------------------------------------------------------------------- | ---- | ----
0x01 | 0x01 | 0x31 0x30 | 0x31 0x30 | 0x41 0x42 0x43 0x44 0x45 0x46 0x47 0x48 0x49 0x4A 0x4B 0x4C 0x4D 0x4E 0x4F 0x50 | 0xC0 | 0x03

#### Resposta:
N�o h�.

Indicador | Descri��o
--------- | ---------
STX | Byte <0x01> de Inicio de Frame
FUNC | Fun��o
X | Posi��o X do LCD (0 - 15)
Y | Posi��o Y do LCD (0 - 1)
TEXTO | 16 Caract�res Alfanum�ricos (ASCII)
CKS | Checksum
ETX | Byte <0x03> de Fim de Frame
