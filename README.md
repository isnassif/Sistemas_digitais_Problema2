# Problema 2 – Integração da API em Assembly com o coprocesador gráfico
<h2>Descrição do Projeto</h2>
<p>
Esse projeto teve como objetivo o desenvolvimento de um módulo para redimensionamento de imagens(zoom e downscale) em um sistema de vigilância de tempo real, utilizando a placa DE1-SoC. 
</p>
<p>
  Na primeira etapa, foi implementado um coprocessador gráfico na FPGA responsável por aplicar os algoritmos de redimensionamento de imagem. No entanto, o redimensionamento era feito por meio de chaves e botões físicos e o carregamento da imagem original na memória por módulos pré definidos no IP Catalog.
</p>
<p>
  A segunda etapa, descrita neste repositório, tem como objetivo automatizar esse controle por meio de software, integrando o Hard Processor System (HPS) ao coprocessador via ponte Avalon (FPGA–HPS). Para isso, foi desenvolvida uma API em Assembly ARM, que envia instruções e dados diretamente à FPGA por meio de endereços mapeados em memória
</p>

<div align="center">
    <img src="Captura de tela 2025-11-06 192558.png"><br>
    <strong>Diagrama introdutório do caminho tomado pelo programa após a API.</strong><br><br>
  </div>

  <h2>Arquitetura e integração do projeto com a API(HPS)</h2>
  <p>
    Na segunda etapa, o principal objetivo foi integrar o processador ARM (HPS) da DE1-SoC com o coprocessador gráfico implementado na FPGA. A comunicação, antes era feita por meio de chaves e botões físicos, além do fato da imagem ser carregada na memória por módulos definidos dentro da IDE Quartus. No projeto atual, essas definições passaram a ocorrer de forma totalmente programável através de uma API escrita em Assembly ARMv7, que acessa diretamente os periféricos da FPGA por meio do barramento AXI Lightweight (AXI-LW).
  </p>
  <h3> Arquitetura Geral da API
  </h3>
  <p>
    A arquitetura da API permite que o software em Assembly ArmV7 executando no HPS, controle as operações do coprocessador gráfico (implementado em hardware na FPGA).
  </p>
  <p>O HPS é responsável por:</p>
  <p>-> Carregar a imagem .mif para uma RAM.</p>
  <p>-> Mapear o barramento AXI-LW, obtendo o endereço virtual dos periféricos da FPGA.</p>
  <p>-> Transferir os dados da imagem para a região de memória compartilhada entre coprocessador e HPS.</p>
  <p>-> Enviar instruções de controle para o coprocessador (como zoom, redução, média, etc.).</p>
  <p>-> Aguardar o término do processamento e liberar recursos.</p>
  <p>A comunicação entre as partes é feita por PIOs(Parallel I/O) configurados no Platform Designer, que são mapeados em endereços físicos dentro do espaço do LW Bridge (0xFF200000).</p>
  <div align="center">
    <img src="diagrama2.png"><br>
    <strong>Fluxo de operação.</strong><br><br>
  </div>
  
