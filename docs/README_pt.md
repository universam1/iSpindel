# Documentação do iSpindle

***A tradução é um trabalho em constante progresso. Fique à vontade para contribuir!
Esta tradução talvez esteja dessincronizada com a versão original escrita em alemão. Por favor, tenha isto em mente antes de ler esta versão.***

## [Components](Parts_pt.md)

## [Calibração](Calibration_pt.md)

## [Diagrama do Cirtuito](circuit_diagram_pt.md)

## [Upload para FHEM](upload-FHEM_pt.md)

## [Script para Ubidots](ubidotsscripting_pt.md)



Documentação do iSpindle (iSpindel)
===================

**Densímetro digital DIY**
***http://www.ispindel.de***


***Por favor, considere a possibilidade de patrocinar este projeto***  

[![Contribua](https://www.paypalobjects.com/en_US/i/btn/btn_donate_LG.gif)](https://www.paypal.me/universam)


![iSpindle in clean water](/pics/floating.jpg)
![Dashboard](/pics/Dashboard.jpg)


## Table of Contents

- [Licença](#licença)
- [Princípio](#princípio)
  - [Peso metacêntrico](#peso-metacêntrico)
- [Construção](#construção)
  - [Componentes](#componentes)
  - [Diagrama do circuito](#diagrama-circuito)
  - [Gaveta](#gaveta)
- [Configuração](#configuração)
  - [Ubidots](#ubidots)
  - [Portal](#portal)
- [Interface Gráfica do Usuário](#interface-gráfica-usuário)
  - [Calibrando o iSpindle](#calibração)
  - [Gráficos Ubidots](#gráficos-ubidots)
  - [CraftBeerPi](#craftbeerpi)
- [Software](#software)


***

## Licença


> Todos os direitos são reservados. Qualquer uso comercial é proibido e violará as patentes aplicáveis.

***

## Princípio

Impulsionada por este tópico [Alternativa ao Spindle](http://hobbybrauer.de/forum/viewtopic.php?f=7&t=11157&view=unread#p170499), a ideia nasceu para reproduzir o produto comercial spindle utilizando componentes de baixo custo.

O sistema é baseado no conceito de inclinação de um cilindro, um conceito ao mesmo tempo simples e engenhoso - você não necessita de nenhuma referência externa (exceto da gravidade) e o cilindro é extremamente simples de limpar. O ângulo de inclinação muda devido ao empuxo e possui relação direta com a concentração de açúcar contida no líquido. Será formado um ângulo entre o centro de massa e o centro do empuxo que é dependente da densidade do líquido.


![Kränung](/pics/kraengung.jpg)

Desta forma a ideia é incluir um dispositivo com funcionalidade Wifi com um acelerômetro e sensor de temperatura dentro de um cilindro que flutuará no líquido. O sistema medirá por meio dos sensores a cada x minutos, conectará à internet via Wifi e enviará informações sobre o ângulo, temperatura e tensão da bateria para um serviço na nuvem como Ubidots.com para armazenamento dos dados.

Com um intervale de update a cada 30 minutos é possível obter uma vida útil da bateria de praticamente 3 meses!

### *Peso metacêntrico*

Este é o conceito de "metacentro": O cilindro mudará o ângulo de inclinação quanda a densidade do líquido mudar em relação ao seu centro de massa e o centro de empuxo. O ângulo de inclinação então poderá ser medido. 

É possível mudar o ângulo de inclinação inicial do cilindro adicional alguns pesos na base do cilindro para que ele fique mais em pé ou na tampa para deixá-lo mais inclinado.

O software calcula o ângulo de Euler devido aos valores de X e Y dos valores de aceleração de XYZ e forma um ângulo absoluto. Nós registramos estes valores com os parâmetros ajustados para ° Plato, SG ou similar correlacionando com as referências medidas.


***

## Construção

### Veja [Componentes](Parts_pt.md)


### Veja [Fontes](Parts_pt.md)

### [Diagrama do Circuito](circuit_diagram_pt.md)
![Schematic](/pics/schematic-pt.png)

***

### Gaveta
- Uma gaveta impressa em 3D é utilizada para fixar todos os componentes além da bateria dentro do cilindro plástico conforme mostrado a seguir. O modelo 3D pode ser obtido no repositório.
![Gaveta](/pics/Schlitten_cad.jpg)
![Montada](/pics/assembled2.jpg)
![Montada](/pics/assembled.jpg)

<a href="http://www.youtube.com/watch?feature=player_embedded&v=gpVarh8BxhQ" target="_blank"><img src="http://img.youtube.com/vi/gpVarh8BxhQ/0.jpg" 
alt="Druck" width="240" height="180" border="10" /></a>



***

## Configuração

### Ubidots

- Para começar, você deve criar uma conta grátis em [Ubidots.com](https://ubidots.com)
- A seguir vá ao menu  ```API Credentials``` e copie uma ```Token``` para ser utilizada pelo iSpindle. O Token funciona como uma autorização concedida ao iSpindel para postar os dados na conta do Ubidots.
***Anote esta chave em algum lugar.***  
![Token](/pics/UbiToken.jpg)  

### Portal

Ao pressionar o botão ```Reset``` da placa Wemos, a mesma criará um ponto de acesso wireless, que permitirá fazer as alterações necessárias para configurar o dispositivo. **No modo `operação` este portal não ficará mais acessível uma vez que o princípio é que o dispositivo fique ativo o menor tempo possível durante o uso. Basicamente o dispositivo acordará, enviará os dados para o portal na nuvem e entrará em modo sleep novamente. Este tempo leva menos de 3 s e possui impacto direto na vida útil da bateria.

> O ```iSpindel``` sinaliza o modo `config` ao piscar o LED a intervalos de 1 s.
Ao salvar as configurações ou após expirar o tempo de 5 minutos o dispositivo encerrará o modo `access point` e iniciará o modo `operation`.


   ![Setup](/pics/configuration.png)

   ![AccessPoint](/pics/AP.png)![Portal](/pics/Portal.png)

  ![Info](/pics/info.png)

> No site Ubidots você pode monitorar os dados enviados na aba ```Sources```, local onde o iSpindel criará um dispositivo por conta própria.  
Na aba ```Dashboard``` você poderá criar diversos gráficos customizados.

***
## Interface Gráfica do Usuário

### Calibração

> Para converter o ângulo medido pelo iSpindle para graus Plato (°Plato), densidade (SG), ou outras unidades, primeiro é necessário calibrar o sensor fazendo várias medições de ângulo a partir de valores de referência de densidade utilizando soluções de água com diferentes concentrações de açúcar. Estes valores de referência serão convertidos a partir de um modelo matemático que será armazenada para futuras medições e visualizações no Ubidots. Tendo em vista que cada dispositivo iSpindle terá diferentes valores medidos (em função dos tamanhos diferentes de tubos, gavetas e etc), cada dispositivo deverá ser calibrado após a montagem para garantir medições precisas. Um procedimento detalhado é mostrado a seguir.

[Procedimento de Calibração](Calibration_pt.md)

### Gráficos Ubidots

- [Fórmula Plato](Calibration_pt.md#formula)

### CraftBeerPi

[CraftBeerPi](https://github.com/universam1/iSpindel/issues/3)
***

## Software 

### Flash do Firmware

[Flash do Firmware](Firmware.md)

***Se gostou, nos escreva para contarw***  :beers:

[![Doe](https://www.paypalobjects.com/de_DE/DE/i/btn/btn_donate_LG.gif)](https://www.paypal.me/universam)