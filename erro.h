/* 
 * 
 */

#ifndef ERRO_H
#define	ERRO_H

typedef enum {
    err_sucesso = 0,
    err_outofmemory,
    err_argumentoInvalido,
    err_naoFezOpen,
    err_chaveOuValorNulo,
    err_adicionarValor,
} errorCode;

// esta função deve ser chamada sempre da forma:
// setError( <codigo>, __LINE__, __FILE__ );

/**
 * Define o ultimo erro
 * @param code Código de erro
 * @param linha __LINE__
 * @param nomeFicheiro __FILE__
 * @return o código de erro definido
 */
int setError(errorCode code, int linha, char *nomeFicheiro);

errorCode getLastError();

void printLastError();

#endif	/* ERRO_H */

