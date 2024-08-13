let atqColor = "";
let dfColor = "";

function escolherTime(tipo, cor) {
    
    if (tipo === 'atq') {
        atqColor = cor;
    } else if (tipo === 'df') {
        dfColor = cor;
    }
    console.log(atqColor);
    console.log(dfColor);
}
function jogar(){
    let atq = atqColor;
    let df = dfColor;  
    let qntAtq = parseInt(document.getElementById("qntAtq").value);
    let qntDf = parseInt(document.getElementById("qntDf").value);


    let dadosAtq = GerarDado(qntAtq);
    let dadosDf = GerarDado(qntDf);

    dadosAtq.sort((a, b) => b - a);
    dadosDf.sort((a, b) => b - a);

    let pAtq = 0;
    let pDf = 0;

    const menor = dadosDf.length >= dadosAtq.length ? dadosAtq : dadosDf;

    for (let i = 0; i < menor.length; i++) {
        if (dadosAtq[i] > dadosDf[i]) {
            pAtq++;
        } else {
            pDf++;
        }
    }

    let result = document.getElementById("result");
    result.innerHTML = `<p>${pAtq > pDf ? atq : df} venceu!</p> <p>Dados do ${atq}: ${dadosAtq.join(', ')}</p> <p>Dados do ${df}: ${dadosDf.join(', ')}</p>`;
	try {
		fetch(`/${pAtq > pDf ? atq : df}`).catch(console.error);
	} catch (ex) {
		console.error(ex.message || ex.toString());
	}
}
function GerarDado(qnt){
    let dado = [];
    for(let i = 0; i < qnt; i++){
        dado.push(Math.floor(Math.random() * 6) + 1); 
    }
    return dado;
}