# Problema 2 – Integração da API em Assembly com o coprocesador gráfico

<nav>
  <h2>Sumário</h2>
  <ul>
    <li><a href="#descricao">Descrição do Projeto</a></li>
    <li><a href="#arquitetura">Arquitetura e Integração do Projeto com a API (HPS)</a></li>
    <li><a href="#comunicacao">Comunicação entre o FPGA e o Computador</a></li>
    <li><a href="#memoria">Estrutura de Memória e Mapeamento</a></li>
    <li><a href="#opcodes">Códigos de Operação (Opcode)</a></li>
    <li><a href="#resumo">Resumo dos arquivos necessários para execução</a></li>
    <li><a href="#funcoes">Principais Funções da API (HPS)</a></li>
    <li><a href="#fluxo">Fluxo Geral de Execução</a></li>
    <li><a href="#analise-resultados">Análise de resultados</a></li>
    <li><a href="#referencias">Referências</a></li>
  </ul>
</nav>


<section id="descricao">
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
    <img src="diagramas/Captura de tela 2025-11-06 192558.png"><br>
    <strong>Diagrama introdutório do caminho tomado pelo programa após a API.</strong><br><br>
  </div>
</section>

<section id="arquitetura">
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
    <img src="diagramas/diagrama2.png"><br>
    <strong>Fluxo de operação.</strong><br><br>
  </div>
  </section>
  <h3>Fluxo de Execução</h3> <p> O funcionamento completo da integração entre o HPS (ARM) e a FPGA pode ser descrito em seis etapas principais, desde o carregamento da imagem até a execução do comando pelo coprocessador gráfico. 
  <strong>1. Mapeamento da Ponte (<i>mapearPonte</i>)</strong><br> O programa em Assembly cria um acesso virtual à área de memória da FPGA por meio da ponte <strong>AXI Lightweight (AXI-LW)</strong>. Dessa forma, os endereços físicos dos periféricos passam a ser acessíveis diretamente pelo software, permitindo o controle do hardware através de ponteiros como <code>CONTROL_PIO_ptr</code> e <code>IMAGE_MEM_ptr</code>. </p> <p><strong>2. Leitura da Imagem (<i>carregarImagemMIF</i>)</strong><br> A rotina lê o arquivo de imagem no formato <strong>.mif</strong>, converte seus valores hexadecimais em bytes de 8 bits e os armazena em um buffer na RAM do HPS. Essa etapa prepara os dados que serão enviados para o coprocessador. </p> <p><strong>3. Transferência para a FPGA (<i>transferirImagemFPGA</i>)</strong><br> O conteúdo do buffer é copiado para a região de memória da FPGA mapeada via <strong>PIO</strong>. Assim, o coprocessador passa a ter acesso direto aos pixels da imagem que será processada. </p> <p><strong>4. Envio do Comando (<i>enviarComando</i>)</strong><br> O HPS escreve no registrador <strong>CONTROL_PIO</strong> o código da operação desejada (por exemplo: zoom in, zoom out, média, etc.). Esse valor é transmitido pelo barramento <strong>AXI-LW</strong>, iniciando a execução da operação na FPGA. </p> <p><strong>5. Execução na FPGA</strong><br> O coprocessador gráfico lê os dados da memória de imagem, aplica o algoritmo correspondente à instrução recebida e envia o resultado processado para a saída VGA, exibindo o efeito visual em tempo real. </p> <p><strong>6. Finalização</strong><br> Após o término da operação, o programa libera os recursos utilizados — desfaz o mapeamento de memória, fecha o descritor de arquivo (<code>/dev/mem</code>) e encerra a execução com segurança. </p> <div align="center"> <img src="fluxo_execucao.png"><br> <strong>Fluxo geral de execução da API em Assembly ARM controlando o coprocessador gráfico.</strong><br><br> </div>

<section id="comunicacao">
  <h2>Comunicação entre o FPGA e o Computador</h2>
    <p>Para que se tornasse possível a comunicação entre o FPGA e o computador, foi necessário utilizar o protocolo HPS (Hard Processor System), responsável por passar as informações do computador para a placa utilizada. Essa troca de informações entre o HPS e a FPGA é realizada por registradores mapeados no espaço de memória da ponte leve (Lightweight Bridge). O protocolo segue três etapas principais: envio da instrução, execução na FPGA e sincronização/finalização.</p>
    <ol>
      <li><strong>Envio do comando de instrução</strong>: o HPS escreve o código da operação desejada (opcode) no registrador de controle (<code>CONTROL_PIO_ptr</code>), no hps, instanciando como pio_led, indicando qual algoritmo a FPGA deve executar.</li>
      <li><strong>Ativação e execução</strong>: a FPGA interpreta o opcode e processa os dados de imagem já presentes na memória compartilhada.</li>
      <li><strong>Sincronização e finalização</strong>: o HPS aguarda um curto intervalo para garantir que a execução na FPGA tenha sido iniciada/concluída e que os dados estejam estáveis.</li>
    </ol>
  </section>

  <section id="memoria">
    <h2>Estrutura de Memória e Mapeamento</h2>
    <p>O HPS acessa diretamente periféricos da FPGA através do mapeamento do dispositivo <code>/dev/mem</code> para a região base da ponte LW. A área mapeada permite leitura/escrita dos registradores de controle e da memória de imagem.</p>

  <table>
      <thead>
        <tr>
          <th>Ponte / Registrador</th>
          <th>Função Principal</th>
        </tr>
      </thead>
      <tbody>
        <tr>
          <td><code>LW_virtual</code></td>
          <td>Endereço virtual resultante do <code>mmap()</code> sobre a região da Lightweight Bridge.</td>
        </tr>
        <tr>
          <td><code>CONTROL_PIO_ptr</code></td>
          <td>Registrador usado para envio de códigos de operação (opcodes) à FPGA.</td>
        </tr>
        <tr>
          <td><code>IMAGE_MEM_ptr</code></td>
          <td>Ponte de memória compartilhada onde a imagem (bytes carregados do <code>.mif</code>) é colocada para que a FPGA leia/processa.</td>
        </tr>
        <tr>
          <td><code>fd</code></td>
          <td>Descriptor do <code>/dev/mem</code> usado para o mapeamento físico.</td>
        </tr>
      </tbody>
    </table>

  <p></strong>Vale ressaltar que, no assembly/C do projeto, a função <code>mapearPonte()</code> realiza a abertura de <code>/dev/mem</code>, chama <code>mmap()</code> e ajusta os ponteiros globais (<code>IMAGE_MEM_ptr</code>, <code>CONTROL_PIO_ptr</code>, etc.) para o espaço virtual retornado pelo <code>mmap</code>.
    </div>
  </section>

<section id="opcodes">
    <h2>Códigos de Operação (Opcode)</h2>
    <p>Os códigos abaixo correspondem às operações implementadas (ou previstas) na FPGA. Estes valores são escritos no registrador de controle para solicitar a operação.</p>

  <table>
      <thead>
        <tr>
          <th>Constante</th>
          <th>Valor</th>
          <th>Descrição</th>
        </tr>
      </thead>
      <tbody>
        <tr>
          <td><code>ST_REPLICACAO</code></td>
          <td><code>0</code></td>
          <td>Replicação 2x (duplica pixels conforme algoritmo FPGA).</td>
        </tr>
        <tr>
          <td><code>ST_DECIMACAO</code></td>
          <td><code>1</code></td>
          <td>Decimação 2x (downsampling / redução).</td>
        </tr>
        <tr>
          <td><code>ST_ZOOMNN</code></td>
          <td><code>2</code></td>
          <td>Zoom por vizinho mais próximo (NN) 2x.</td>
        </tr>
        <tr>
          <td><code>ST_MEDIA</code></td>
          <td><code>3</code></td>
          <td>Filtro de média (suavização) 2x.</td>
        </tr>
        <tr>
          <td><code>ST_COPIA_DIRETA</code></td>
          <td><code>4</code></td>
          <td>Cópia direta da imagem (transferência sem processamento).</td>
        </tr>
        <tr>
          <td><code>ST_RESET</code></td>
          <td><code>7</code></td>
          <td>Reinicializa o estado interno do coprocessador FPGA.</td>
        </tr>
        <tr>
          <td><code>ST_REPLICACAO4</code></td>
          <td><code>8</code></td>
          <td>Replicação 4x.</td>
        </tr>
        <tr>
          <td><code>ST_DECIMACAO4</code></td>
          <td><code>9</code></td>
          <td>Decimação 4x.</td>
        </tr>
        <tr>
          <td><code>ST_ZOOMNN4</code></td>
          <td><code>10</code></td>
          <td>Zoom NN 4x.</td>
        </tr>
        <tr>
          <td><code>ST_MED4</code></td>
          <td><code>11</code></td>
          <td>Média 4x.</td>
        </tr>
      </tbody>
    </table>

  <p>Cada um desses opcodes é decocificado e associado a uma sequência de números binários, responsável por ativar cada uma dos algoritmos, vale ressaltar que, o bit mais significativo é o responsável por definir o fator do algoritmo, os demais, a seleção do algoritmo.</p>
  <section id="resumo">
  <h2>Resumo dos arquivos necessários para execução.</h2>
  <h3>api.h</h3>
  <p>Função: Cabeçalho que declara todas as funções e variáveis globais da API Assembly.

-> Expõe os símbolos definidos em Assembly (EXPECTED_IMG_WIDTH, hps_img_buffer, mapearPonte, etc.).

-> Serve de “ponte” entre o código C (main.c) e as implementações em Assembly (funcoes.s).</p>
<h3>hps_0.h</h3>
<p>Função: Cabeçalho gerado automaticamente pelo Qsys/Platform Designer.
-> Define os endereços base de cada periférico do sistema (PIO_LED, ONCHIP_MEMORY2_1, etc.).

-> O main.c lê ONCHIP_MEMORY2_1_BASE e PIO_LED_BASE daqui.</p>
<h3>makefile</h3>
<p>Função: Automatiza o processo de compilação, execução e limpeza do projeto, simplificando o uso no terminal</p>
<h3>main.c</h3>
<p>Função: Aplicação de controle em alto nível.Interface de escolha.</p>
<h3>funcoes.s(API)</h3>
<p>Função: Implementação completa da API em Assembly ARMv7, com integração direta ao hardware via /dev/mem. No próximo tópico as funções desse arquivo serão mais apronfudadas.</p>
  </section>

  <section id="funcoes">
    <h2>Principais Funções da API (HPS)</h2>
    <p>A lógica HPS é composta por rotinas em Assembly (armv7) e código C que orquestram o fluxo. A seguir estão descrições resumidas das funções exportadas e seu comportamento esperado.</p>

<h3>carregarImagemMIF</h3>
    <p>Abre o arquivo <code>.mif</code>, ignora linhas de cabeçalho (palavras-chave como CONTENT, BEGIN, END, ADDRESS_RADIX, DATA_RADIX, WIDTH, DEPTH) e converte linhas de dados hexadecimais em bytes que são armazenados em um buffer alocado dinamicamente. Retorna o número de bytes lidos ou valor negativo em caso de erro.</p>

  <h3>mapearPonte</h3>
    <p>Abre <code>/dev/mem</code>, faz <code>mmap()</code> da região base da LW Bridge (endereço base <code>0xFF200000</code>, span <code>0x30000</code>) e calcula os ponteiros virtuais para <code>IMAGE_MEM_ptr</code> e <code>CONTROL_PIO_ptr</code> com base nos offsets configurados (valores definidos em <code>hps_0.h</code>). Em caso de falha retorna erro negativo.</p>

  <h3>transferirImagemFPGA</h3>
    <p>Copia os bytes do buffer HPS (onde <code>carregarImagemMIF</code> depositou os dados) para a região mapeada de memória da FPGA (<code>IMAGE_MEM_ptr</code>), normalmente usando <code>memcpy</code> em Assembly/C.</p>

  <h3>enviarComando</h3>
    <p>Escreve o opcode recebido no endereço apontado por <code>CONTROL_PIO_ptr</code> para solicitar a operação correspondente na FPGA. Faz uma barreira de memória (dmb sy) e um pequeno <code>usleep</code> para sincronização.</p>

  <h3>limparRecursos</h3>
    <p>Libera o buffer de imagem alocado, faz munmap da região mapeada e fecha o descriptor do <code>/dev/mem</code>, garantindo que não haja vazamentos de recursos ao final da execução.</p>
  </section>

<section id="fluxo">
    <h2>Fluxo Geral de Execução</h2>
    <p>O processo completo de uso do sistema segue a ordem abaixo:</p>
    <ol>
      <li>Inicialização: o HPS carrega e inicializa variáveis base (valores em hps_0.h), e chama mapearPonte para preparar a comunicação.</li>
      <li>Carregamento da imagem: carregarImagemMIF lê o arquivo imagem.mif e aloca um buffer com os bytes da imagem.</li>
      <li>Transferência: transferirImagemFPGA copia os bytes para a memória da FPGA.</li>
      <li>Execução: o usuário (interface em main.c) seleciona a operação via menu; o HPS obtém o opcode e chama enviarComando.</li>
      <li>Processamento: a FPGA executa o algoritmo correspondente e atualiza a memória (ou sinaliza conclusão via flags, dependendo da implementação FPGA).</li>
      <li>Finalização: após as operações, limparRecursos é chamado para desmapear e liberar recursos.</li>
    </ol>
  </section>

  <section id="analise-resultados">
  <h2>Análise dos Resultados Alcançados</h2>
   <p>
    Durante a fase de testes, verificou-se que as rotinas de escrita e leitura dos registradores mapeados em memória responderam de maneira previsível e confiável. O envio de instruções pelo registrador de controle e a leitura dos sinais de status demonstraram total compatibilidade com a lógica desenvolvida em Verilog, confirmando o correto alinhamento entre hardware e software. A temporização aplicada entre os comandos também se mostrou adequada, permitindo a conclusão das operações de forma estável e sem interferências entre ciclos consecutivos.
  </p>

  <p>
    O sistema de carregamento de imagem em formato <em>.mif</em> funcionou conforme esperado, possibilitando a leitura completa do arquivo e o envio dos dados para a FPGA. Essa etapa foi essencial para validar o fluxo de comunicação e o endereçamento correto da memória compartilhada, garantindo que os dados processados correspondessem integralmente à imagem original. O controle de execução das operações foi testado por meio de diferentes códigos de operação, cada um correspondendo a funções específicas no coprocessador, e todos apresentaram resultados consistentes com a lógica projetada.
  </p>

  <p>
    Além da validação funcional, o comportamento do sistema demonstrou baixo tempo de resposta e alta previsibilidade, características fundamentais para aplicações de processamento em tempo real. A execução direta em Assembly contribuiu para uma comunicação de baixo nível mais precisa, eliminando camadas intermediárias e permitindo maior controle sobre o hardware.
  </p>

  <p>
    Como possibilidades de aprimoramento, destaca-se a implementação de um controle mais sofisticado para a transferência de blocos de dados, permitindo o envio parcial de regiões específicas da imagem. Também se prevê o desenvolvimento de uma camada de software em linguagem C ou C++, destinada a oferecer uma interface interativa ao usuário, facilitando a seleção de comandos e a análise dos resultados diretamente no HPS. Por fim, a integração com novos algoritmos de processamento — como filtragem, espelhamento e replicação — poderá expandir as capacidades do sistema, aproximando-o de um ambiente completo de manipulação de imagens em hardware.
  </p>

  <p>
    Os resultados obtidos comprovam a eficiência do modelo proposto, demonstrando que a arquitetura baseada na cooperação entre HPS e FPGA é capaz de executar operações de processamento de imagem de maneira confiável, rápida e totalmente sincronizada. Dessa forma, o sistema desenvolvido estabelece uma base sólida para implementações futuras e aplicações de maior complexidade no campo do processamento digital de imagens em hardware reconfigurável.
  </p>
</section>

<section id="referencias">
  <h2>Referências</h2>
  <ol>
    <li>ALTERA. <em>DE1-SoC Computer System with ARM Cortex-A9 and FPGA Fabric – Technical Reference Manual.</em> Intel Corporation, 2019.</li>
    <li>INTEL. <em>Embedded Peripherals IP User Guide.</em> Intel FPGA Documentation, 2021. Disponível em: 
      <a href="https://www.intel.com/content/www/us/en/programmable/documentation.html" target="_blank">
        https://www.intel.com/content/www/us/en/programmable/documentation.html
      </a>.
    </li>
    <li>BARE-METAL Programming on ARM Cortex-A9 (HPS–FPGA System). Universidade Federal de Campina Grande – Laboratório de Sistemas Digitais, 2022.</li>
    <li>ARM Limited. <em>ARM Architecture Reference Manual: ARMv7-A and ARMv7-R edition.</em> ARM, 2012.</li>
    <li>INTEL FPGA University Program. <em>DE1-SoC Computer and Qsys System Design Tutorial.</em> Intel Education, 2020.</li>
    <li>IEEE Computer Society. <em>Standard for SystemVerilog—Unified Hardware Design, Specification, and Verification Language (IEEE Std 1800-2017).</em> IEEE, 2017.</li>
  </ol>
</section>
