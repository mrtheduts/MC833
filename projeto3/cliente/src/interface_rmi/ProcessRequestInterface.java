package interface_rmi;

import java.rmi.*;

//  (1) listar todas as pessoas formadas em um determinado curso;(2) listar as habilidades dos perfis que moram em uma determinada cidade;(3) acrescentar uma nova experiência em um perfil;(4) dado o email do perfil, retornar sua experiência;(5) listar todas as informações de todos os perfis;(6) dado o email de um perfil, retornar suas informações.

public interface ProcessRequestInterface extends Remote {

    public String list_name_course(String course) throws RemoteException;
    public String list_hab_city(String city) throws RemoteException;
    public String add_exp(String email, String work_location, String job) throws RemoteException;
    public String list_exp_email(String email) throws RemoteException;
    public String list_all() throws RemoteException;
    public String list_all_email (String email) throws RemoteException;
}

