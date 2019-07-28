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

        String remoteHostName = args[0];
        int remotePort = 1099;
        String connectLocation = "rmi://" + remoteHostName + ":" + remotePort + "/Worker";

        ProcessRequestInterface worker = null;

        Scanner sc = new Scanner(System.in);

        try {
            System.out.println("Connecting to client at :" + connectLocation);
            // worker = (ProcessRequestInterface) Naming.lookup(connectLocation);
        
            Registry registry = LocateRegistry.getRegistry(remoteHostName, remotePort);
            worker = (ProcessRequestInterface) registry.lookup("Worker");

            char op;


            do {

                long t_start;
                long t_finish;

                System.out.println(INIT_MSG);
                String msgToServer;

                op = sc.next().charAt(0);


                System.out.println("Você escolheu a opção " + op);

                if (op == '1') {


                    System.out.println("Digite o curso: ");
                    msgToServer = sc.next();

                    t_start = System.nanoTime();

                    System.out.println(worker.list_name_course(msgToServer));

                    t_finish = System.nanoTime();
                    System.out.println("Consulta realizada em:\n" + "|" + (t_finish - t_start));
                }
                else if (op == '2') {

                    System.out.println("Digite a cidade: ");
                    msgToServer = sc.next();
                    t_start = System.nanoTime();
                    System.out.println(worker.list_hab_city(msgToServer));
                    t_finish = System.nanoTime();
                    System.out.println("Consulta realizada em:\n" + "|" + (t_finish - t_start));
                }
                else if (op == '3') {

                    System.out.println("Digite o email: ");
                    msgToServer = sc.next();

                    System.out.println("Digite o local: ");
                    String work_location = sc.next();

                    System.out.println("Digite o cargo: ");
                    String job = sc.next();

                    t_start = System.nanoTime();
                    System.out.println(worker.add_exp(msgToServer, work_location, job));
                    t_finish = System.nanoTime();
                    System.out.println("Consulta realizada em:\n" + "|" + (t_finish - t_start));
                }
                else if (op == '4') {

                    System.out.println("Digite o email: ");
                    msgToServer = sc.next();
                    t_start = System.nanoTime();
                    System.out.println(worker.list_exp_email(msgToServer));
                    t_finish = System.nanoTime();
                    System.out.println("Consulta realizada em:\n" + "|" + (t_finish - t_start));
                }
                else if (op == '5') {

                    t_start = System.nanoTime();
                    System.out.println(worker.list_all());
                    t_finish = System.nanoTime();
                    System.out.println("Consulta realizada em:\n" + "|" + (t_finish - t_start));
                }
                else if (op == '6') {

                    System.out.println("Digite o email: ");
                    msgToServer = sc.next();
                    t_start = System.nanoTime();
                    System.out.println(worker.list_all_email(msgToServer));
                    t_finish = System.nanoTime();
                    System.out.println("Consulta realizada em:\n" + "|" + (t_finish - t_start));
                }
                if (op == '7') {

                    System.out.println("Volte sempre :)");
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

