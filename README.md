# SOSDriver SmartBot

Robô inteligente de monitoramento ambiental para emergências veiculares, desenvolvido para a atividade CitizenTech (Global Solution, FIAP). O SmartBot é a evolução do robô criado na disciplina de MakerLab: ele lê sensores, envia os dados a um backend em Python e usa um LLM (Gemini) para interpretar o contexto e devolver uma orientação clara ao motorista.

* Hardware simulado no Wokwi. Link do projeto no Wokwi: https://wokwi.com/projects/481127863533020161
* Link do vídeo no youtube: https://youtu.be/xxyyfLUX-Ak

### Sumário
* Problema e proposta
* Visão geral da arquitetura
* Componentes utilizados
* Estrutura do repositório
* Instalação
* Execução
* Integração entre sensores e IA
* Código-fonte
* Testes realizados
* Limitações e próximos passos


## 1. Problema e proposta

Um motorista parado em uma situação de risco (fumaça, calor excessivo, obstáculo muito próximo) muitas vezes não sabe como agir. O SmartBot monitora o ambiente do veículo, identifica condições de alerta e fornece orientações contextualizadas em linguagem natural, integrando-se à plataforma SOSDriver de assistência veicular.

Evolução em relação ao robô do MakerLab:

* Sensores adicionados: [PREENCHER: quais sensores são novos em relação ao robô do MakerLab]
* Integração com IA: o robô agora envia as leituras a um LLM, que interpreta o ambiente e recomenda ações.
* Comunicação em rede: ESP32 conectado por Wi-Fi a um backend próprio.


## 2. Visão geral da arquitetura

<img width="633" height="361" alt="image" src="https://github.com/user-attachments/assets/61d517ad-fcd1-4fe8-936e-2305c436cd4b" />


# SOSDriver SmartBot

Sistema inteligente de monitoramento ambiental e apoio ao motorista em situações de emergência veicular, desenvolvido como componente auxiliar da plataforma **SOSDriver**.

<img width="520" height="428" alt="image" src="https://github.com/user-attachments/assets/4c0f3a0f-f089-4d29-bfaa-d2df0ba4bc0e" />


## 1. Sobre o projeto

O **SOSDriver SmartBot** é um protótipo de robô IoT desenvolvido com ESP32 e sensores ambientais. Seu objetivo é monitorar condições que possam representar riscos durante uma situação de emergência veicular, como temperatura elevada, possível presença de gás ou fumaça e obstáculos próximos.

O sistema combina sensores, processamento local, comunicação com um backend em Python e inteligência artificial por meio do **Google Gemini**, que gera recomendações contextualizadas ao motorista.

O projeto foi desenvolvido e testado em ambiente de simulação utilizando o **Wokwi**.

## 2. Como funciona

O funcionamento do SOSDriver SmartBot é dividido em três etapas principais:

- **Monitoramento:** o ESP32 coleta os dados dos sensores e verifica localmente as condições monitoradas.
- **Processamento:** as leituras são enviadas ao backend, que identifica riscos e consulta o Gemini.
- **Orientação:** a inteligência artificial gera recomendações curtas e objetivas para auxiliar o motorista.

### Fluxo resumido

1. O ESP32 lê os sensores e decide localmente o estado (`NORMAL`, `ALERTA` ou `CRÍTICO`), acionando OLED, LED e buzzer.
2. A cada 8 segundos, envia as leituras em JSON ao backend (`POST /sensores`).
3. O backend monta um prompt com as leituras e consulta o Gemini.
4. O Gemini devolve uma orientação curta ao motorista. Se a IA falhar ou demorar, o backend usa **regras fixas** como plano B.
5. O backend responde com os dados e a recomendação, e guarda a última leitura para consulta.

## 3. Componentes utilizados

### Hardware (simulado no Wokwi)

| Componente | Função | Pino (GPIO) |
|---|---|---|
| ESP32 DevKit | Unidade central de processamento e Wi-Fi | — |
| DHT22 | Temperatura e umidade | 15 |
| MQ-2 | Gases e fumaça (leitura analógica) | 34 |
| HC-SR04 | Distância de obstáculos | TRIG 19, ECHO 18 |
| Display OLED SSD1306 (I2C) | Exibe leituras e situação | SDA 21, SCL 22 |
| LED vermelho | Sinaliza alerta | 2 |
| Buzzer | Alerta sonoro em situação crítica | 4 |

### Limites de alerta


| Condição | Limite |
|---|---|
| Alerta de gás (MQ-2) | `>= 2500` |
| Gás crítico (buzzer) | `>= 2600` |
| Alerta de temperatura | `>= 45 °C` |
| Temperatura crítica (buzzer) | `>= 60 °C` |
| Alerta de obstáculo | `<= 30 cm` |

### Software

| Tecnologia | Uso |
|---|---|
| C++ (Arduino/ESP32) | Firmware do robô |
| Python 3.10+ | Backend |
| FastAPI + Uvicorn | API REST |
| Pydantic | Validação dos dados dos sensores |
| python-dotenv | Leitura da chave de API (`.env`) |
| Biblioteca `openai` | Cliente usado para chamar o Gemini pelo endpoint compatível |
| Google Gemini (`gemini-flash-lite-latest`, com `gemini-flash-latest` de reserva) | Interpretação do contexto e recomendação |
| ngrok | Túnel público para o Wokwi alcançar o backend local |

**Bibliotecas do Wokwi (`libraries.txt`):**

- `DHT sensor library for ESP32`
- `Adafruit GFX Library`
- `Adafruit SSD1306`

As bibliotecas `WiFi`, `WiFiClientSecure` e `HTTPClient` já vêm com o ESP32.

## 4. Estrutura do repositório

```text
SOSDriver/
├── backend/
│   ├── main.py             # API FastAPI
│   ├── ai_service.py       # Integração com o Gemini + plano B por regras
│   ├── requirements.txt    # Dependências Python
│   └── .env.example        # Modelo do arquivo de configuração
├── .gitignore
└── README.md
```

## 5. Instalação

### Pré-requisitos

- Python 3.10 ou superior.
- Uma chave gratuita da API do Gemini: [Google AI Studio](https://aistudio.google.com/apikey). Sem a chave, o sistema funciona com as regras fixas.
- Conta gratuita no [ngrok](https://ngrok.com), para o robô simulado enviar dados ao backend local.
- Conta no [Wokwi](https://wokwi.com), para abrir o circuito.

### Backend

Clone o repositório e acesse a pasta do backend:

```bash
git clone <URL-DO-REPOSITORIO>
cd SOSDriver/backend
```

Crie e ative o ambiente virtual:

```bash
python -m venv .venv
```

**Windows (PowerShell):**

```powershell
..\.venv\Scripts\Activate.ps1
```

**Linux / macOS:**

```bash
source .venv/bin/activate
```

Instale as dependências:

```bash
pip install -r requirements.txt
```

### Configuração da chave Gemini

Crie o arquivo `backend/.env` a partir do modelo `.env.example`:

```env
GEMINI_API_KEY=cole_sua_chave_aqui
```

### Configuração do ngrok

Instale o ngrok e registre seu token de autenticação:

```bash
ngrok config add-authtoken SEU_TOKEN
```

O token é configurado uma única vez por ambiente.

## 6. Execução

Inicie os serviços na ordem indicada. Cada serviço deve permanecer aberto em um terminal separado.

### 1. Backend

Na pasta `backend`, com o ambiente virtual ativado, execute:

```bash
python -m uvicorn main:app --port 8000
```

Aguarde a mensagem `Application startup complete`.

A documentação interativa da API estará disponível em:

[http://localhost:8000/docs](http://localhost:8000/docs)

### 2. Túnel ngrok

Em outro terminal, execute:

```bash
ngrok http 8000 --scheme http,https
```

Copie o domínio público exibido na linha `Forwarding`, por exemplo:

```text
algo.ngrok-free.dev
```

### 3. Robô no Wokwi

Abra o projeto no Wokwi e edite as duas configurações no início do arquivo `sketch.ino`:

```cpp
const char* HOST = "algo.ngrok-free.dev";  // Domínio do ngrok, sem https:// e sem barra
const bool USAR_HTTPS = false;             // Configuração utilizada na simulação
```

Substitua o domínio de exemplo pelo domínio atual do seu ngrok e inicie a simulação.

Quando o monitor serial apresentar `Resposta HTTP 200`, significa que a requisição chegou ao backend e foi processada.

> **Nota sobre HTTP:** na simulação, a conexão HTTPS com o túnel apresentou falha (`connection refused`). Por isso, o ngrok é iniciado com `--scheme http,https` e o firmware utiliza HTTP. Em uma implantação real, a comunicação deve utilizar HTTPS com validação adequada do certificado.

### 4. Consultar os resultados

| Endereço | Função |
|---|---|
| [http://localhost:8000/](http://localhost:8000/) | Verifica se o backend está online |
| [http://localhost:8000/ultimo](http://localhost:8000/ultimo) | Exibe a última leitura e sua recomendação |
| [http://localhost:8000/historico](http://localhost:8000/historico) | Exibe as últimas 20 leituras |
| [http://127.0.0.1:4040](http://127.0.0.1:4040) | Painel do ngrok com as requisições recebidas |

### Testar sem o Wokwi

Também é possível testar o backend diretamente pela documentação interativa:

1. Acesse [http://localhost:8000/docs](http://localhost:8000/docs).
2. Localize `POST /sensores`.
3. Clique em **Try it out**.
4. Envie o seguinte JSON:

```json
{
  "temperatura": 50.0,
  "umidade": 40.0,
  "gas": 2800,
  "distancia": 120.0
}
```

5. Clique em **Execute** para visualizar a resposta.

## 7. Integração entre sensores e IA

### 7.1 Leitura e tratamento no ESP32

- **DHT22:** realiza a leitura de temperatura e umidade. Se algum valor vier como `NaN`, o envio é ignorado, pois `NaN` não é um valor JSON válido.
- **MQ-2:** realiza a leitura analógica inteira, com valores entre 0 e 4095.
- **HC-SR04:** mede a distância em centímetros. Quando não recebe eco, o sensor devolve `-1`; o firmware envia `400` (alcance máximo), para o backend não interpretar a falha como um obstáculo muito próximo.
- **Decisão local:** o ESP32 compara as leituras com os limites configurados e aciona OLED, LED e buzzer sem depender da conexão com a internet.

Assim, o robô continua realizando os alertas locais mesmo quando o backend ou a inteligência artificial estão indisponíveis.

### 7.2 Contrato da API

O endpoint `POST /sensores` recebe os dados dos sensores em formato JSON.

| Campo | Tipo | Descrição |
|---|---|---|
| `temperatura` | float | Temperatura em °C |
| `umidade` | float | Umidade relativa em % |
| `gas` | int | Leitura analógica do MQ-2 |
| `distancia` | float | Distância do obstáculo em cm |

**Exemplo de envio feito pelo ESP32:**

```json
{
  "temperatura": 18.3,
  "umidade": 53.5,
  "gas": 843,
  "distancia": 222.9
}
```

**Exemplo de resposta:**

```json
{
  "status": "dados recebidos",
  "dados": {
    "temperatura": 18.3,
    "umidade": 53.5,
    "gas": 843,
    "distancia": 222.9
  },
  "recomendacao": "Temperatura, umidade e gás em níveis seguros. Há um obstáculo a mais de dois metros, sem risco imediato.",
  "horario": "2026-11-18T10:19:40.603450"
}
```

**Outros endpoints:**

| Rota | Método | Função |
|---|---|---|
| `/` | GET | Verifica se o backend está online |
| `/sensores` | POST | Recebe leituras e devolve a recomendação |
| `/ultimo` | GET | Consulta a última leitura recebida (em memória) |
| `/historico` | GET | Consulta as últimas 20 leituras (em memória) |

Campos ausentes ou com tipos incorretos geram resposta HTTP `422`, devido à validação do Pydantic.

### 7.3 Interpretação pela IA

Para cada leitura recebida, o backend monta um prompt que:

- Define o papel do modelo como assistente de emergência veicular.
- Informa os valores coletados pelos sensores.
- Apresenta os limites de alerta.
- Solicita uma orientação curta, clara e objetiva, com no máximo três frases.

**Exemplo simplificado do prompt:**

```text
Você é o SOSDriver SmartBot, assistente de emergência veicular. Com base nas
leituras abaixo, identifique riscos e dê orientações curtas e claras ao
motorista (máximo 3 frases).

Temperatura: 18.3 °C (alerta a partir de 45)
Umidade: 53.5 %
Gás/fumaça (MQ-2): 843 (alerta a partir de 2500)
Distância do obstáculo: 222.9 cm (alerta até 30)
```

A chamada utiliza o endpoint do Gemini compatível com a biblioteca `openai`, por meio do parâmetro `base_url` apontando para `generativelanguage.googleapis.com`.

A biblioteca `openai` atua apenas como cliente HTTP: as requisições são enviadas ao Google utilizando a chave `GEMINI_API_KEY`.

### 7.4 Tolerância a falhas

O backend possui mecanismos alternativos para evitar que uma falha na IA interrompa o funcionamento do sistema.

| Situação | Comportamento |
|---|---|
| Modelo principal falha ou excede 8 segundos | Tenta o modelo de reserva |
| Os dois modelos falham | Utiliza as **regras fixas** para gás, temperatura e distância |
| Sem `GEMINI_API_KEY` | Utiliza diretamente as regras fixas |
| Sem Wi-Fi no ESP32 | Tenta reconectar a cada ciclo; LED, buzzer e OLED continuam funcionando |

O limite de tempo é de 8 segundos por modelo, para que a resposta seja recebida dentro do período de 20 segundos que o ESP32 aguarda pela resposta HTTP.

## 8. Código-fonte

Os arquivos completos estão disponíveis no repositório. Abaixo estão os principais arquivos do backend.

<details>
<summary><strong>backend/main.py</strong></summary>

```python
from fastapi import FastAPI
from datetime import datetime
from pydantic import BaseModel
from ai_service import gerar_recomendacao

app = FastAPI(
    title="SOSDriver API",
    description="Backend do SOSDriver SmartBot",
    version="1.0.0"
)

class DadosSensores(BaseModel):
    temperatura: float
    umidade: float
    gas: int
    distancia: float

ultima_leitura = None
historico = []

@app.get("/")
def inicio():
    return {
        "projeto": "SOSDriver SmartBot",
        "status": "online",
        "mensagem": "Backend funcionando!"
    }

@app.post("/sensores")
def receber_dados(dados: DadosSensores):
    global ultima_leitura
    d = dados.model_dump()
    resultado = {
        "status": "dados recebidos",
        "dados": d,
        "recomendacao": gerar_recomendacao(d),
        "horario": datetime.now().isoformat()
    }
    ultima_leitura = resultado
    historico.append(resultado)
    del historico[:-20]  # guarda só as 20 últimas
    return resultado

@app.get("/ultimo")
def ultimo():
    return ultima_leitura or {"mensagem": "Nenhum dado recebido ainda"}

@app.get("/historico")
def ver_historico():
    return historico
```

</details>

<details>
<summary><strong>backend/ai_service.py</strong></summary>

```python
import os
from dotenv import load_dotenv
from openai import OpenAI

load_dotenv()

api_key = os.getenv("GEMINI_API_KEY")
client = OpenAI(
    api_key=api_key,
    base_url="https://generativelanguage.googleapis.com/v1beta/openai/",
    timeout=8.0,
    max_retries=0,
) if api_key else None

MODELOS = ["gemini-flash-lite-latest", "gemini-flash-latest"]


def recomendacao_por_regras(dados: dict) -> str:
    avisos = []

    if dados["gas"] > 2500:
        avisos.append(
            "Possível presença de gás ou fumaça. Desligue o motor, saia do veículo "
            "e afaste-se, sem acender chamas."
        )
    if dados["temperatura"] > 45:
        avisos.append(
            "Temperatura elevada. Evite permanecer dentro do veículo "
            "e procure sombra e ventilação."
        )
    if dados["distancia"] < 30:
        avisos.append(
            "Obstáculo muito próximo. Não manobre sem verificar o entorno "
            "e sinalize o veículo."
        )

    if not avisos:
        return "Condições normais. Nenhum risco identificado no momento."

    return " ".join(avisos) + " Se necessário, acione o socorro pelo SOSDriver."


def gerar_recomendacao(dados: dict) -> str:
    if client is None:
        return recomendacao_por_regras(dados)

    prompt = (
        "Você é o SOSDriver SmartBot, assistente de emergência veicular. "
        "Com base nas leituras abaixo, identifique riscos e dê orientações "
        "curtas e claras ao motorista (máximo 3 frases).\n\n"
        f"Temperatura: {dados['temperatura']} °C (alerta a partir de 45)\n"
        f"Umidade: {dados['umidade']} %\n"
        f"Gás/fumaça (MQ-2): {dados['gas']} (alerta a partir de 2500)\n"
        f"Distância do obstáculo: {dados['distancia']} cm (alerta até 30)"
    )

    for modelo in MODELOS:
        try:
            resposta = client.chat.completions.create(
                model=modelo,
                messages=[{"role": "user", "content": prompt}],
                max_tokens=400,
            )
            texto = resposta.choices[0].message.content
            if texto and texto.strip():
                return texto.strip()
        except Exception as e:
            print(f"[ai_service] {modelo} falhou: {e}")

    print("[ai_service] Usando regras fixas.")
    return recomendacao_por_regras(dados)
```

</details>

<details>
<summary><strong>backend/requirements.txt</strong></summary>

```text
fastapi
uvicorn
python-dotenv
openai
```

</details>

<details>
<summary><strong>backend/.env.example</strong></summary>

```env
GEMINI_API_KEY=cole_sua_chave_aqui
```

</details>

### Firmware do ESP32

O firmware está disponível em `wokwi/sketch.ino`.

Ele realiza as seguintes operações:

- Lê os sensores.
- Aplica os limites de alerta.
- Controla o OLED, LED e buzzer.
- Envia os dados ao backend a cada 8 segundos, por meio da função `enviarParaBackend()`.

## 9. Testes realizados

| Teste | Entrada | Resultado |
|---|---|---|
| Funcionamento normal | Temp. 18,3 °C, umidade 53,5%, MQ-2 843, distância 222,9 cm | NORMAL; IA indica condições normais |
| Alerta por gás | MQ-2 acima de 2500 | ALERTA (LED aceso); CRÍTICO acima de 2600 (buzzer) |
| Alerta de proximidade | Distância de 1,99 cm | ALERTA; IA orienta sobre o obstáculo |
| Integração ponta a ponta | ESP32 (Wokwi) → ngrok → FastAPI → Gemini | `Resposta HTTP 200` com recomendação |
| Falha da IA | Timeout ou erro do Gemini | Recomendação gerada pelas regras fixas |

## 10. Limitações e próximos passos

### Limitações

- Os limites dos sensores são parâmetros de simulação e ainda **não foram calibrados** para uso real.
- O hardware é simulado no Wokwi; o HTTPS com o túnel não funcionou no simulador, por isso o teste utiliza HTTP.
- A integração com a plataforma SOSDriver ainda não foi realizada; esta etapa entrega o robô e seu backend.
- A última leitura e o histórico ficam em memória e são perdidos quando o servidor reinicia.
- A recomendação da IA é um apoio e não substitui o acionamento de serviços de emergência.

### Próximos passos

- Calibrar os sensores com hardware real.
- Enviar a variação das leituras ao modelo, para identificar tendências (por exemplo, gás subindo).
- Exibir a recomendação no OLED do robô.
- Integrar com a plataforma SOSDriver e utilizar HTTPS.
- Persistir o histórico em banco de dados.

---

## 11. Segurança e observações

- Não publique o arquivo `.env` nem tokens de autenticação.
- Mantenha as chaves de API fora do código-fonte.
- O túnel ngrok é utilizado para testes e desenvolvimento, não como solução definitiva de produção.
- Os sensores e limites precisam de validação física antes de qualquer utilização em situações reais.
- O SmartBot é um recurso auxiliar de monitoramento e orientação, não um sistema certificado de segurança veicular.

---

**SOSDriver SmartBot — Projeto MakerLab / CitizenTech**
