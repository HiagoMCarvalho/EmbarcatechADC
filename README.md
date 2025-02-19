# Embarcatech ADC

## Descrição do Projeto
Este projeto implementa o controle de um quadrado 8x8 mostrado em um display utilizando um joystick, juntamente com a manipulação de leds via pwm e interrupção

## Estrutura do Projeto

```
ADC/
├── .vscode/            # Configurações do VS Code (opcional)
├── build/              # Diretório de build (gerado pelo CMake)
├── .gitignore          # Arquivo para ignorar arquivos no controle de versão
├── CMakeLists.txt      # Configuração do CMake para compilação do projeto
├── pico_sdk_import.cmake # Importação do SDK do Raspberry Pi Pico
├── adc.c               # Código principal do projeto
├── font.h              # Desenhos pre definidos pela biblioteca
├── ssd1306.h           # Header para definição de funções
├── ssd1306.c           # Implementação das funções definidas no header
```

## Dependências
Para compilar e rodar este projeto, você precisará de:
- Raspberry Pi Pico SDK
- CMake
- Um ambiente de desenvolvimento C (GCC ARM)

## Compilação e Execução
1. Configure o ambiente do SDK do Raspberry Pi Pico.
2. No terminal, navegue até o diretório do projeto:
   ```sh
   git clone https://github.com/HiagoMCarvalho/EmbarcatechADC.git
   cd ADC
   ```
3. Crie um diretório de build e entre nele:
   ```sh
   mkdir build && cd build
   ```
4. Execute o CMake para configurar o projeto:
   ```sh
   cmake ..
   ```
5. Compile o projeto:
   ```sh
   make
   ```
6. Envie o arquivo `.uf2` gerado para o Raspberry Pi Pico.


## Funcionamento do Código
- O joystick controla o quadrado através de conversor analógico digital
- Os leds vermelho e azul acompanham os valores do joystick e alteram suas intensidades através de pwm
- O led verde é manipulado através de interrupção, juntamente com a adição de uma nova borda para o display

## Autor
Desenvolvido por <https://github.com/HiagoMCarvalho>

## Vídeo
Assista ao vídeo explicação: <https://drive.google.com/file/d/1JwFyW98LI8cmTXwCK5-mHoSeJNLGmaXs/view?usp=sharing>

## Licença
Este projeto está sob a licença MIT.

