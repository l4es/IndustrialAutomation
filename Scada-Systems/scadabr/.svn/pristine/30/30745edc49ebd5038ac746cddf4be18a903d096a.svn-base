package br.ufsc.edugraf.rose;

import java.io.IOException;
import java.io.InputStream;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.Properties;
import org.restlet.data.CharacterSet;

public class Configuracoes
{
  public static final String URL_BASE = "urlBase";
  public static final String SENHA_DO_CERTIFICADO = "senhaDoCertificado";
  public static final String RAIZ_DO_CERTIFICADO = "raizDoCertificado";
  public static final String ARQUIVO_DE_CONFIGURACAO = "../classes/restAPI.properties";
  public static final String EM_PRODUCAO = "emProdução";
//  private static final int PORTA_PADRÃO_HTTP = 80;
  private static Properties propriedades;

  protected Configuracoes()
  {
    obterPropriedades();
  }

  private static Properties obterPropriedades() {
    if (propriedades == null) {
      propriedades = new Properties();
      try
      {
        InputStream fis = Configuracoes.class.getClassLoader().getResourceAsStream("../classes/restAPI.properties");
        propriedades.load(fis);
      } catch (IOException e) {
        System.out.println("Problema ao ler o arquivo de configurações");
        e.printStackTrace();
      }
    }
    return propriedades;
  }

  public static URI obterRaizDoProjeto() {
    URI raiz = null;
    try {
      raiz = new URI(Configuracoes.class.getClassLoader().getResource("../classes/restAPI.properties").getPath());
    } catch (URISyntaxException e) {
      e.printStackTrace();
    }
    return raiz;
  }

  public static URI obterDiretórioDaClasse(Class<?> umaClasse) {
    try {
      return umaClasse.getResource(".").toURI();
    }
    catch (URISyntaxException e) {
      e.printStackTrace();
    }
    return null;
  }

  public static String obterComoTexto(String propriedade) {
    return propriedades.getProperty(propriedade, "");
  }

  public static String obterComoTexto(String propriedade, String padrao) {
    return propriedades.getProperty(propriedade, padrao);
  }

  public static int obterComoNumero(String propriedade) {
    String valor = propriedades.getProperty(propriedade, "0");
    return Integer.parseInt(valor);
  }

  public static int obterComoNumero(String propriedade, String padrao) {
    String valor = propriedades.getProperty(propriedade, padrao);
    return Integer.parseInt(valor);
  }

  public static boolean obterComoBooleano(String propriedade) {
    String valor = propriedades.getProperty(propriedade, "false");
    return Boolean.parseBoolean(valor);
  }

  public static boolean obterComoBooleano(String propriedade, boolean padrao) {
    String valor = propriedades.getProperty(propriedade, null);
    return valor == null ? padrao : Boolean.parseBoolean(valor);
  }

  public static Integer obterPortaDoServiço() {
    try {
      int porta = new URI(obterComoTexto("urlBase")).getPort();
      return Integer.valueOf(porta == -1 ? 80 : porta); } catch (URISyntaxException e) {
    }
    throw new RuntimeException("URL base no arquivo de configurações é inválida: " + obterComoTexto("urlBase"));
  }

  public static String obterURLBase()
  {
    return obterComoTexto("urlBase") + "/";
  }

  public static URI obterUriRelativaRaizDoProjeto(String caminhoRelativo) {
    return obterRaizDoProjeto().resolve(obterComoTexto(caminhoRelativo));
  }

  public static CharacterSet obterCodificaçãoPadrão() {
    return CharacterSet.UTF_8;
  }
}