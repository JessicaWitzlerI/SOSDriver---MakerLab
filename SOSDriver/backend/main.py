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