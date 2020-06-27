# serialmonitor
Monitor de Portas Seriais para Windows

![screenshot](https://raw.githubusercontent.com/fcsgehrke/serialmonitor/master/img/screenshot.png)

# Protocolo:

O protocolo é bem simples. A estrutura é composta dos bytes abaixo


## Estrutura:

Nome | Byte | Tamanho
------------------------
STX | <0x01> | 01
FUNÇÃO | <0x02> | 01
DADOS | <....> | 20
CKS | <....> | 01
ETX | <0x03> | 01


## Comandos:

Comando | Byte
--------------
Ler I/O | <0x01>
Escrever I/O | <0x02>
Escrever LCD | <0x03>
Limpar LCD | <0x04>
Escrever PWM | <0x05>

### Ler I/O
