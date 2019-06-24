import java.net.MalformedURLException;

import java.rmi.*;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;

import java.util.Scanner;

import interface_rmi.ProcessRequestInterface;

//  (1) listar todas as pessoas formadas em um determinado curso;(2) listar as habilidades dos perfis que moram em uma determinada cidade;(3) acrescentar uma nova experiência em um perfil;(4) dado o email do perfil, retornar sua experiência;(5) listar todas as informações de todos os perfis;(6) dado o email de um perfil, retornar suas informações.
public class Client {

    private static String INIT_MSG = "\n" +
        "Olá! As opções disponíveis são:\n\n" + 
        "(1) Listar todas as pessoas formadas em um determinado curso\n" +
        "(2) Listar as habilidades dos perfis que moram em uma determinada cidade\n" +
        "(3) Acrescentar uma nova experiẽncia em um perfil\n" +
        "(4) Dado o email de um perfil, retorna sua experiência\n" +
        "(5) Lista todas as informações de todos os perfis\n" +
        "(6) Dado o email de um perfil, retorna suas informações\n\n" +
        "(7) Sair\n\n";

    private static String ERROR = "Isso não é um comando válido\n\n";

    public static void main (String[] args) {

        System.setProperty("java.security.policy", "security.policy");
        System.setSecurityManager(new SecurityManager());

        String remoteHostName = "casawje.ddns.net";
        int remotePort = 1099;
        String connectLocation = "rmi://" + remoteHostName + ":" + remotePort + "/Worker";

        ProcessRequestInterface worker = null;

        try {
            System.out.println("Connecting to client at :" + connectLocation);
            // worker = (ProcessRequestInterface) Naming.lookup(connectLocation);
        
            Registry registry = LocateRegistry.getRegistry(remoteHostName, remotePort);
            worker = (ProcessRequestInterface) registry.lookup("Worker");

            char op;

            do {

                String msgToServer;
                Scanner sc = new Scanner(System.in);

                op = sc.next().charAt(0);

                System.out.println("Você escolheu a opção " + op);

                if (op == '1') {


                    System.out.println("Digite a cidade: ");
                    msgToServer = sc.next();
                    System.out.println(worker.list_all_city(msgToServer));
                }
                else if (op == '2') {

                    System.out.println("Digite a cidade: ");
                    msgToServer = sc.next();
                    System.out.println(worker.list_hab_city(msgToServer));
                }
                else if (op == '3') {

                    System.out.println("Digite o email: ");
                    msgToServer = sc.next();

                    System.out.println("Digite a experiencia: ");
                    String exp_to_add = sc.next();

                    System.out.println(worker.add_exp(msgToServer, exp_to_add));
                }
                else if (op == '4') {

                    System.out.println("Digite o email: ");
                    msgToServer = sc.next();
                    System.out.println(worker.list_exp_email(msgToServer));
                }
                else if (op == '5') {

                    System.out.println(worker.list_all());
                }
                else if (op == '6') {

                    System.out.println("Digite o email: ");
                    msgToServer = sc.next();
                    System.out.println(worker.list_all_email(msgToServer));
                }
                if (op == '7') {

                    System.out.println("É isso aí.");
                }

            } while (op != '7');

        } catch (Exception e) {
            System.out.println("Client Exception: ");
            e.printStackTrace();
        }
        

//         ProcessRequestInterface worker;

//         try {

//             System.setProperty("java.security.policy", "security.policy");
//             System.setSecurityManager(new SecurityManager());
//             worker = (ProcessRequestInterface) Naming.lookup("//casawje.ddns.net:22222/Worker");

//             System.out.println(worker.hello());

//         } catch (Exception e) {
//             System.out.println("Client error: " + e);
//         }
    }
}

