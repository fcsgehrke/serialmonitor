# serialmonitor
Monitor de Portas Seriais para Windows

![screenshot](https://raw.githubusercontent.com/fcsgehrke/serialmonitor/master/img/screenshot.png)

# Protocolo:

O protocolo é bem simples. A estrutura é composta dos bytes abaixo. Os frames tem tamanho fixo de 24 bytes. A pergunta e a resposta tem a mesma estrutura. O checksum é calculado iniciando no segundo byte e é bem simples, apenas um soma de todos os bytes.

## Estrutura:

Nome | Byte | Tamanho
---- | ---- |--------
STX | <0x01> | 01
FUNÇÃO | <0x02> | 01
DADOS | <....> | 20
CKS | <....> | 01
ETX | <0x03> | 01


## Funções:

Função | Byte
------- | ----
Ler I/O | <0x01>
Escrever I/O | <0x02>
Escrever LCD | <0x03>
Limpar LCD | <0x04>
Escrever PWM | <0x05>

### Ler I/O
#### Envio:
0x01 0x01 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0xC0 0x03
-STX-FUNC---------------------------------------------- DADOS -------------------------------------------------CKS-ETX-

#### Resposta:
STX | FUNC | O1 | O2 | O3 | O4 | I1 | I2 | I3 | I4 | PWMC | PWMD | PWMU | SOBRA | CKS | ETX
--- | ---- | -- | -- | -- | -- | -- | -- | -- | -- | ---- | ---- | ---- | ----- | --- | ---
0x01 | 0x01 | 0x31 | 0x30 | 0x31 | 0x30 | 0x30 | 0x30 | 0x30 | 0x30 | 0x30 | 0x30 | 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 0x30 | 0xC0 | 0x03


Indicador | Descrição
--------- | ---------
STX | Byte <0x01> de Inicio de Frame
FUNC | Função
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
