package server;

import java.rmi.*;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.*;
import java.net.MalformedURLException;


public class Server {

    public static void main (String[] argv) throws RemoteException {

        // System.setProperty("java.rmi.server.hostname","192.168.0.111");

        try {
            System.setProperty("java.security.policy", "/home/mrtheduts/Git/unicamp/mc833/projeto3/servidor/src/main/java/security.policy");
            System.setSecurityManager(new SecurityManager());

            int port = 1099;
            ProcessRequest Worker = new ProcessRequest();
            // ProcessRequestInterface stub = (ProcessRequestInterface) UnicastRemoteObject.exportObject(Worker, port);

            Registry registry = LocateRegistry.createRegistry(port);
            System.out.println("java RMI registry created.");

            String hostname = "0.0.0.0";
            String bindLocation = "rmi://" + hostname + ":" + port + "/Worker";

            // Registry registry = LocateRegistry.getRegistry();
            // registry.rebind(bindLocation, stub);

            Naming.bind(bindLocation, Worker);
            System.out.println("Server is ready at:" + bindLocation);
        } catch (Exception e) {
            System.out.println("Server exception: ");
            e.printStackTrace();
        }

    }
}
         
