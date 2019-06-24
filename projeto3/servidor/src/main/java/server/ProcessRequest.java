package server;

import interface_rmi.ProcessRequestInterface;

import java.rmi.*;
import java.rmi.server.*;

import java.net.UnknownHostException;
import java.util.List;
import java.util.Set;

import com.mongodb.DB;
import com.mongodb.MongoClient;
import com.mongodb.DBCollection;
import com.mongodb.DBObject;
import com.mongodb.BasicDBObject;
import com.mongodb.DBCursor;

public class ProcessRequest extends UnicastRemoteObject
            implements ProcessRequestInterface {

            MongoClient mongoClient;
            DB db;
            DBCollection collection;

            public ProcessRequest () throws RemoteException {

                try {

                    this.mongoClient = new MongoClient("localhost", 27017);

                } catch (Exception e) {
                    System.out.println("Mongodb error: " + e);
                }

                this.db = mongoClient.getDB("projeto_redes");
                this.collection = db.getCollection("pessoas");
            }

            public String list_all_city(String city) throws RemoteException{

                DBCursor results = collection.find(new BasicDBObject("Residencia", city));
                return "list_all_city";
            }

            public String list_hab_city(String city) throws RemoteException{

                return "list_hab_city";
            }

            public String add_exp(String email, String exp_to_add) throws RemoteException{

                return "Experiencia adicionada com sucesso!";
            }

            public String list_exp_email(String email) throws RemoteException{

                return "list_exp_email";
            }

            public String list_all() throws RemoteException{

                return "list_all";
            }

            public String list_all_email (String email) throws RemoteException{

                return "list_all_email";
            }

}
