
import os
from dotenv import load_dotenv
from openai import OpenAI

load_dotenv()

client = OpenAI(
    api_key=os.getenv("GEMINI_API_KEY"),
    base_url="https://generativelanguage.googleapis.com/v1beta/openai/",
    timeout=15.0,
    max_retries=0,
)

MODELO = "gemini-flash-lite-latest"

# Limites de segurança do SmartBot
TEMP_ALERTA = 45
GAS_ALERTA = 2500
DISTANCIA_ALERTA = 30


def identificar_riscos(dados: dict) -> list:
    riscos = []

    if dados["temperatura"] > TEMP_ALERTA:
        riscos.append(
            f"Temperatura elevada: {dados['temperatura']} °C "
            f"(limite de alerta: {TEMP_ALERTA} °C)."
        )

    if dados["gas"] > GAS_ALERTA:
        riscos.append(
            f"Possível presença de gás ou fumaça: "
            f"{dados['gas']} (limite de alerta: {GAS_ALERTA})."
        )

    if dados["distancia"] < DISTANCIA_ALERTA:
        riscos.append(
            f"Obstáculo próximo: {dados['distancia']} cm "
            f"(limite de alerta: {DISTANCIA_ALERTA} cm)."
        )

    return riscos


def recomendacao_por_regras(dados: dict) -> str:
    riscos = identificar_riscos(dados)

    if not riscos:
        return (
            "Condições normais nos parâmetros monitorados. "
            "Mantenha a atenção e verifique as condições do veículo."
        )

    return (
        "ATENÇÃO! " + " ".join(riscos) +
        " Siga as orientações de segurança e, se necessário, "
        "solicite assistência pelo SOSDriver."
    )


def gerar_recomendacao(dados: dict) -> str:
    riscos = identificar_riscos(dados)

    if not riscos:
        contexto = "Nenhum dos limites de alerta foi ultrapassado."
    else:
        contexto = (
            "RISCOS IDENTIFICADOS PELO SISTEMA:\n"
            + "\n".join(riscos)
        )

    prompt = f"""
Você é o SOSDriver SmartBot, assistente de emergência veicular.

Analise os dados recebidos e produza uma recomendação curta,
clara e segura para o motorista.

DADOS DOS SENSORES:
Temperatura: {dados['temperatura']} °C
Umidade: {dados['umidade']} %
Gás/fumaça (MQ-2): {dados['gas']}
Distância do obstáculo: {dados['distancia']} cm

ANÁLISE DETERMINÍSTICA DO SISTEMA:
{contexto}

REGRAS OBRIGATÓRIAS:
1. Nunca classifique como normal um parâmetro que ultrapassou
   seu limite de alerta.
2. Considere todos os riscos identificados pelo sistema.
3. Não invente problemas que não foram identificados.
4. Não afirme que o veículo está totalmente seguro.
5. Forneça orientações práticas e objetivas.
6. Use no máximo 3 frases.
"""

    try:
        resposta = client.chat.completions.create(
            model=MODELO,
            messages=[{"role": "user", "content": prompt}],
            max_tokens=300,
        )

        texto = resposta.choices[0].message.content

        if texto and texto.strip():
            # A IA complementa os riscos, mas não os substitui.
            if riscos:
                return (
                    "ATENÇÃO! "
                    + " ".join(riscos)
                    + "\n\n"
                    + texto.strip()
                )

            return texto.strip()

        return recomendacao_por_regras(dados)

    except Exception as e:
        print(f"[ai_service] Falha na IA, usando regras fixas: {e}")
        return recomendacao_por_regras(dados)
