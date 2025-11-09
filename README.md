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
