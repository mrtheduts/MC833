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
import com.mongodb.BasicDBList;

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

            public String list_name_course(String course) throws RemoteException{

                DBCursor results = collection.find(new BasicDBObject("Formacao Academica", course));
                String output = "{[";

                for (DBObject result : results) {

                    output += (result.get("Nome Completo").toString()) + ",";
                }

                output = output.substring(0, output.length() - 1);
                output += "]}";

                return output;
            }

            public String list_hab_city(String city) throws RemoteException{

                DBCursor results = collection.find(new BasicDBObject("Residencia", city));
                String output = "{[";

                for (DBObject result : results) {

                    output += (result.get("Habilidades").toString()) + ",";
                }
                output = output.substring(0, output.length() - 1);
                output += "]}";
                return output;
            }

            public String add_exp(String email, String work_location, String job) throws RemoteException{

                DBCursor results = collection.find(new BasicDBObject("Email", email));
                DBObject person = results.one();

                BasicDBList exp = (BasicDBList)person.get("Experiencia");

                BasicDBObject new_exp = new BasicDBObject("Local", work_location);
                new_exp.put("Cargo", job);

                exp.add(0, new_exp);

                person.put("Experiencia", exp);
                System.out.println(person.toString());

                collection.update(new BasicDBObject("Email", email), person);

                return "Experiência adicionada!";
            }

            public String list_exp_email(String email) throws RemoteException{

                DBCursor results = collection.find(new BasicDBObject("Email", email));
                String output = "{[";

                for (DBObject result : results) {

                    output += (result.get("Experiencia").toString()) + ",";
                }
                output = output.substring(0, output.length() - 1);
                output += "]}";
                return output;
            }

            public String list_all() throws RemoteException{

                DBObject fields = new BasicDBObject("_id", 0);
                fields.put("senha", 0);
                DBCursor results = collection.find(new BasicDBObject(), fields);

                String output = "{[";

                for (DBObject result : results) {

                    output += (result.toString()) + ",";
                }
                output = output.substring(0, output.length() - 1);
                output += "]}";
                return output;
            }

            public String list_all_email (String email) throws RemoteException{

                DBObject fields = new BasicDBObject("_id", 0);
                fields.put("senha", 0);
                DBCursor results = collection.find(new BasicDBObject("Email", email), fields);

                String output = "{[";

                for (DBObject result : results) {

                    output += (result.toString()) + ",";
                }
                output = output.substring(0, output.length() - 1);
                output += "]}";
                return output;
            }

}
