# 💡 PoC Smart Lamp - FIWARE + ESP32 + LDR

Trabalho desenvolvido para a disciplina **Edge Computing** do curso de **Engenharia de Software** da **FIAP**.  

Este projeto demonstra a integração de um **ESP32** equipado com sensor **LDR** ao ecossistema **FIWARE**, utilizando **Azure**, **MQTT**, **Orion Context Broker**, **Postman** e simulação no **Wokwi**.  

A solução implementa uma **Smart Lamp** capaz de:  
- Enviar dados de luminosidade (edge → cloud).  
- Receber comandos de ligar/desligar LED (cloud → edge).  
- Operar via IoT Agent MQTT integrado ao Orion.  

---

## 🛠️ Tecnologias Utilizadas
- **FIWARE Descomplicado** (IoT Agent MQTT + Orion Context Broker)  
- **Microsoft Azure** (VM para hospedar containers)  
- **Docker & Docker Compose**  
- **ESP32 DEVKIT V1** + **Sensor LDR**  
- **Wokwi** (simulação do hardware)  
- **Postman** (testes de API REST)  
- **MQTT** (comunicação entre ESP32 e FIWARE)  

---

## ⚙️ Instalação no Azure

1. Criar uma **VM Linux** no Azure (tamanho mínimo B1s).  
2. Acessar a VM via SSH.  
3. Instalar o FIWARE Descomplicado:
   ```bash
   git clone https://github.com/fabiocabrini/fiware
   cd fiware
   sudo docker-compose up -d
   ```
4. Após a inicialização, os seguintes serviços ficam disponíveis:  
   - IoT Agent MQTT → porta `4041`  
   - Orion Context Broker → porta `1026`  
   - STH-Comet (histórico) → porta `8666`  

---

## 📡 ESP32 + LDR

O ESP32 lê valores de luminosidade através de um **LDR** (pino 34) e publica no tópico MQTT.  
Também recebe comandos do FIWARE para ligar/desligar o LED onboard (GPIO 2).  

- Código-fonte disponível no repositório: [`sketch.ino`](./sketch.ino)  
- Simulação disponível no Wokwi: [🔗 Link da simulação](https://wokwi.com/projects/441395890230460417)  

---

## 🧪 Testes com Postman

A coleção do Postman está disponível em [`FIWARE Descomplicado.postman_collection.json`](./FIWARE%20Descomplicado.postman_collection.json).  

Principais requisições:  
1. **Health Check** – verifica se o IoT Agent está ativo.  
2. **Provision Service Group** – cria grupo para dispositivos MQTT (apikey `TEF`).  
3. **Provision Smart Lamp** – registra a entidade `Lamp:001`.  
4. **Register Commands** – registra comandos `on` e `off`.  
5. **List Devices** – lista todos dispositivos provisionados.  
6. **Switching on the Lamp (PATCH)** – envia comando para acender/desligar LED.  
7. **Get Luminosity** – consulta última leitura do LDR.  
8. **Get State** – consulta estado atual do LED.  
9. **Delete Device** – remove o dispositivo no IoT Agent.  
10. **Delete Entity** – remove a entidade no Orion.  

---

## 📺 Evidências

- [🎥 Vídeo Time-lapse (3min)](link_para_o_video) – demonstração do envio de dados de luminosidade e comando de ligar/desligar LED via Postman.  
- [🔗 Wokwi Simulation](https://wokwi.com/projects/441395890230460417)

<img width="1161" height="592" alt="image" src="https://github.com/user-attachments/assets/24fab67b-6ec9-4d53-90df-403426d83496" />

<img width="1917" height="950" alt="image" src="https://github.com/user-attachments/assets/bcb82bda-37a4-4407-b8c6-63f8c470b77d" />

<img width="1919" height="1041" alt="image" src="https://github.com/user-attachments/assets/e1279ae5-1b29-491a-927d-d2ccbdf4571a" />

<img width="1163" height="662" alt="image" src="https://github.com/user-attachments/assets/253b114e-bcb6-4889-a16f-50cfdc7f56a3" />

---

## 👨‍💻 Autores

- Pedro Alves Faleiros - 562523  
- Luan Felix - 565541  
- João Lopes - 565737  
- Leandro Farias - 566488  

---

## 📚 Referências

- [FIWARE Descomplicado - GitHub](https://github.com/fabiocabrini/fiware)  
- [Wokwi Simulator](https://wokwi.com/)  
- [FIWARE Docs](https://fiware.org/developers/)  

---

## ✅ Conclusão

Este projeto demonstrou como **Edge Devices** podem ser integrados a plataformas de **Smart Data Models** utilizando o ecossistema **FIWARE**, validando a interoperabilidade de aplicações IoT em um ambiente de **cloud + edge**.  
