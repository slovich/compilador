# Projeto Compilador
Desenvolvimento dos entregáveis da disciplina compiladores do curso de Engenharia da Computação da UFMA

## Uso de Inteligência Artificial no Desenvolvimento

### Modelo de IA utilizado

O desenvolvimento deste projeto contou com o auxílio do **ChatGPT, da OpenAI**, utilizando o modelo de linguagem **GPT-6**, como ferramenta de apoio à programação, análise de código, resolução de problemas técnicos e documentação.

A IA foi utilizada como assistente de desenvolvimento, auxiliando na implementação incremental das funcionalidades e na compreensão dos conceitos envolvidos na construção de um compilador para a linguagem Jack.

### Papel da IA no projeto

A utilização da IA ocorreu de forma colaborativa e iterativa, com o desenvolvedor responsável pelas decisões técnicas, pela condução do projeto, pela execução dos procedimentos sugeridos e pela validação dos resultados obtidos.

As principais contribuições da IA até esta etapa foram:

- **Apoio à arquitetura e à organização do código:** auxílio na estruturação das classes, na definição de responsabilidades dos componentes e na integração dos módulos desenvolvidos em C++, Qt Widgets e CMake.
- **Desenvolvimento do analisador léxico:** suporte à implementação e ao aprimoramento dos mecanismos de identificação de tokens, incluindo palavras-chave, símbolos, constantes inteiras, constantes de texto e identificadores.
- **Desenvolvimento do analisador sintático:** auxílio na implementação da estratégia de análise sintática recursiva descendente, com procedimentos associados às regras gramaticais da linguagem Jack.
- **Geração da representação XML:** suporte à construção da estrutura hierárquica do XML sintático, respeitando a organização gramatical dos elementos reconhecidos pelo analisador.
- **Tratamento de erros:** auxílio na implementação e no aprimoramento das mensagens de erro sintático, incluindo a identificação da linha e da coluna em que os problemas são encontrados.
- **Integração com a interface gráfica:** orientação na integração dos analisadores com a interface Qt, incluindo a visualização do XML, a representação hierárquica da árvore sintática, o realce de sintaxe e a exportação dos resultados.
- **Depuração e resolução de problemas:** análise de mensagens de erro, investigação de comportamentos inesperados e proposição de correções incrementais, com compilação e testes realizados pelo desenvolvedor.
- **Testes e validação:** elaboração de casos de teste com códigos Jack válidos e inválidos para verificar o reconhecimento de construções gramaticais e o comportamento do analisador diante de erros.
- **Controle de versão e documentação:** auxílio na elaboração de mensagens de commit, na resolução de problemas de sincronização com o GitHub e na documentação das funcionalidades implementadas para os releases do projeto.

### Metodologia de utilização

A IA foi utilizada como ferramenta de apoio durante um processo de desenvolvimento incremental. Em vez de solicitar a produção integral da aplicação em uma única etapa, o desenvolvedor conduziu a implementação por funcionalidades, analisando as orientações recebidas e aplicando as alterações de forma gradual.

O fluxo de trabalho adotado envolveu:

1. Definição da funcionalidade a ser implementada e de seus requisitos.
2. Discussão da abordagem técnica e das alterações necessárias.
3. Apresentação de orientações, trechos de código ou procedimentos pela IA.
4. Aplicação das alterações pelo desenvolvedor no ambiente de desenvolvimento.
5. Compilação e execução da aplicação para verificar o comportamento implementado.
6. Apresentação de erros ou resultados à IA para análise e correção, quando necessário.
7. Registro das alterações no controle de versão após a implementação e a validação.

Essa metodologia permitiu acompanhar a evolução do projeto de maneira organizada, mantendo a compreensão do código e das decisões técnicas como parte do processo de desenvolvimento.

### Responsabilidades e limitações

A IA atuou como ferramenta auxiliar, não como substituta do desenvolvedor. A definição dos objetivos, a condução das etapas, a aplicação das modificações, a execução dos testes e a administração do repositório permaneceram sob responsabilidade do desenvolvedor.

As sugestões produzidas pela IA não foram consideradas automaticamente corretas. O código e os procedimentos propostos precisaram ser avaliados no contexto do projeto e, quando aplicável, verificados por meio de compilação e testes.

Até esta etapa, a aplicação contempla os analisadores léxico e sintático, a geração e a visualização do XML sintático, a representação hierárquica da árvore sintática e os recursos de realce e exportação implementados. A análise semântica e a geração de código para a máquina virtual Jack permanecem fora do escopo das funcionalidades concluídas nesta fase.

### Considerações finais

A utilização da IA contribuiu para acelerar a implementação de funcionalidades, apoiar a investigação de problemas e facilitar a compreensão de conceitos relacionados à construção de compiladores e ao desenvolvimento de aplicações com Qt.

O projeto também serviu como oportunidade de aprendizado prático, permitindo ao desenvolvedor acompanhar e avaliar as decisões de implementação, compreender a integração entre os componentes e utilizar ferramentas modernas de assistência à programação de forma crítica e consciente.
