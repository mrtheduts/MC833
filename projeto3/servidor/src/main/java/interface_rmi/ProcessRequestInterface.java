package interface_rmi;

import java.rmi.*;

public interface ProcessRequestInterface extends Remote {
        public String list_name_course(String course) throws RemoteException;
        public String list_hab_city(String city) throws RemoteException;
        public String add_exp(String email, String work_location, String job) throws RemoteException;
        public String list_exp_email(String email) throws RemoteException;
        public String list_all() throws RemoteException;
        public String list_all_email (String email) throws RemoteException;
}
