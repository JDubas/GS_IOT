
# Projeto IoT: Controle Inteligente de Janelas para Economia de Energia

Este projeto utiliza a placa **ESP32** e o serviço **Thinger.io** para criar um sistema inteligente de controle de janelas. O objetivo é otimizar o consumo de energia, reduzindo a dependência de ar-condicionado ao regular a temperatura interna de forma eficiente.


### Video explicando o projeto:
https://youtu.be/FPqf9qtWW0A
---

## 🎯 **Objetivo**
Criar um sistema IoT que:
- Permita ao usuário definir uma **temperatura ideal** através do portal do **Thinger.io**.
- Controle automaticamente as janelas para ajustar a temperatura interna.
- Monitore as condições climáticas (como chuva) para garantir segurança e eficiência.
- Reduza o consumo de energia elétrica ao priorizar métodos passivos de ventilação antes de acionar o ar-condicionado.

---

## 🛠️ **Como Funciona**

1. **Configuração Inicial**  
   O usuário acessa o portal **Thinger.io** e define uma **temperatura ideal** para o ambiente.

2. **Monitoramento de Condições Climáticas**  
   A **ESP32** monitora:  
   - A **temperatura atual** do ambiente interno, usando um sensor de temperatura e umidade com o **DHT22**.  
   - Se está **chovendo**, utilizando um sensor de chuva ou outro método de detecção.

3. **Controle Inteligente das Janelas**  
   - Se a temperatura atual está acima da temperatura ideal, o sistema tenta reduzir a temperatura interna **abrindo as janelas**.  
   - Caso esteja chovendo, as janelas permanecem **fechadas** para evitar danos.  
   - O sistema verifica continuamente se a ventilação natural está suficiente para alcançar a temperatura desejada.

4. **Ação Complementar**  
   - Se a ventilação natural não for suficiente, o sistema sinaliza que o ar-condicionado pode ser ligado, mantendo o uso mínimo e eficiente.

5. **Resultados e Economia de Energia**  
   O sistema prioriza o uso de ventilação natural, reduzindo o consumo de eletricidade e contribuindo para um ambiente mais sustentável.

---
