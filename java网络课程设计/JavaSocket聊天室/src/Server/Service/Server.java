package Server.Service;

import SharedModule.*;

import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.io.*;
import java.net.Socket;
import java.util.*;

public class Server {
    private HashMap<String, UserInfo> regUsersInfo = new HashMap<>(200); //��ע���û���Ϣ
    private HashSet<String> usedNickname = new HashSet<>(200); //��ʹ���û��ǳ�
    private final HashMap<String, ClientIO> onlineUserIO = new HashMap<>(); //�����û�����TCP�׽��ֵ����������
    private HashMap<String, ArrayList<UserInfo>> friendShip = new HashMap<>(); //���ѹ�ϵ �û�ID-�û���Ϣ
    private DefaultTableModel onlineUsersTableModel; //�����������û���ʾ�������
    private JTextArea feedbackArea;
    public Server() throws IOException {
        loadServerData();  //��������
    }
    //����ͻ�������
    public void processRequest(Socket curSocket){
        try{
            ObjectInputStream ois = new ObjectInputStream(curSocket.getInputStream());
            ObjectOutputStream oos = new ObjectOutputStream(curSocket.getOutputStream());
            ClientIO userIO = new ClientIO(ois, oos);
            boolean flag = true;
            while(flag){ //��ͣ�ض�ȡ�ͻ��˷��������������
                //�������������ж�ȡ���ͻ����ύ���������
                Request clientRe = (Request) ois.readObject();
                System.out.println("------------------");
                System.out.println("Server��ȡ�˿ͻ��˵�����,������ʽ = " + clientRe.getAction());
                switch (clientRe.getAction()) {
                    case SIGN_IN:
                        System.out.println("�����¼");
                        dealSignIn(clientRe, userIO);
                        break;
                    case SIGN_UP:
                        System.out.println("����ע��");
                        dealSignUp(clientRe, userIO);
                        break;
                    case SEND_MESSAGE:
                        System.out.println("������Ϣ����");
                        dealSendMessage(clientRe);
                        break;
                    case LOG_OUT:
                        System.out.println("�����û�����");
                        dealLogout(clientRe, userIO);
                        flag = false;//�û����ߣ�ֹͣ��ȡ����
                        break;
                    case ADD_FRIEND:
                        System.out.println("������Ӻ���");
                        dealAddFriend(clientRe, userIO);
                        break;
                }
                System.out.println("------------------");
            }
        }catch(Exception ignore){
            ignore.printStackTrace();
        }
    }
    
    //�����¼
    public void dealSignIn(Request clientRequest, ClientIO userIO) throws IOException {
        String userID = (String) clientRequest.getDataByKey("userID");
        String password = (String) clientRequest.getDataByKey("password");
        Response response;
        if (!regUsersInfo.containsKey(userID)){  //�û���������
            response = new Response(ResponseStatus.OK, ResponseType.WRONG_ID);
            sendResponse(response, userIO.getOos());//������Ӧ
        } else if (!regUsersInfo.get(userID).getPassword().equals(password)){  //���벻��ȷ
            response = new Response(ResponseStatus.OK, ResponseType.WRONG_PWD);
            sendResponse(response, userIO.getOos());
        } else if(onlineUserIO.containsKey(userID)) { //�ظ���¼
            response = new Response(ResponseStatus.OK, ResponseType.SECOND_LOGIN);
            sendResponse(response, userIO.getOos());
        } else{   //��½�ɹ�
            System.out.println("��¼�ɹ�");
            response = new Response(ResponseStatus.OK, ResponseType.SUCCESS_SIGN_IN);
            response.addData("userInfo", regUsersInfo.get(userID));  //�����û���Ϣ
            ArrayList<UserInfo> friends = friendShip.get(userID);
            response.addData("userFriends", friends);  //�����û������б�
            
            //��������û��б�
            ArrayList<UserInfo> onlineFriends = new ArrayList<>();
            if (friends != null) {
                for (UserInfo curUser : friends) 
                    if (onlineUserIO.containsKey(curUser.getUserID())) 
                        onlineFriends.add(curUser);
            }
            response.addData("userOnlineFriends", onlineFriends);  //�����û����ߺ����б�
            System.out.println("���ݼ������");
            sendResponse(response, userIO.getOos());//������Ӧ
            System.out.println("���ͳɹ�");
            onlineUserIO.put(userID, userIO);//�������û�IO Map����Ӹ��û������������
            addUserToOnlineUsersTable(userID, regUsersInfo.get(userID).getNickName());//���·����������û���ʾ

            //֪ͨ�������ߺ��ѣ����û�����
            response = new Response(ResponseStatus.OK, ResponseType.FRIEND_LOGIN);
            response.addData("userInfo", regUsersInfo.get(userID));
            if (friends != null) {
                for (UserInfo receiveUser : friends) 
                    if (onlineUserIO.containsKey(receiveUser.getUserID()))
                        sendResponse(response, onlineUserIO.get(receiveUser.getUserID()).getOos());
            }
        }
        System.out.println("����������ɴ����¼");
    }
    
    //����ע��
    public void dealSignUp(Request clientRequest, ClientIO userIO) throws IOException{
        UserInfo user = (UserInfo) clientRequest.getDataByKey("user");
        if (usedNickname.contains(user.getNickName())){//�ǳ��Ѿ�����
            Response response = new Response(ResponseStatus.OK, ResponseType.NICKNAME_EXIST);
            ObjectOutputStream oos = userIO.getOos();
            sendResponse(response, oos);  //������Ӧ
        }else {
            synchronized (this.regUsersInfo) {  //����
                Calendar c = Calendar.getInstance();
                String userID;
                //��������˺�
                do {
                    StringBuilder account = new StringBuilder();
                    account.append(String.valueOf(c.get(Calendar.YEAR)), 2, 4);
                    account.append(c.get(Calendar.MONTH) + 1);
                    account.append(c.get(Calendar.DATE));
                    account.append(c.get(Calendar.HOUR));
//                    sb.append(c.get(Calendar.MINUTE));
//                    sb.append(c.get(Calendar.SECOND));
                    account.append(new Random().nextInt(1000));
                    userID = account.toString();
                } while (regUsersInfo.containsKey(userID));
                //�����˺�
                user.setUserID(userID);
                System.out.println("�����˺�Ϊ��" + userID);
                regUsersInfo.put(userID, user); //��ӵ���ע���û�
                usedNickname.add(user.getNickName()); //��ӵ���ʹ��nickname
                friendShip.put(userID, new ArrayList<>());//�����պ��ѹ�ϵ
                Response response = new Response(ResponseStatus.OK, ResponseType.SUCCESS_SIGN_UP);
                response.addData("userID", userID);
                ObjectOutputStream oos = userIO.getOos();
                sendResponse(response, oos);//������Ӧ
                System.out.println("����������ɴ���ע��");
            }
        }
    }
    
    //������Ϣ����
    public void dealSendMessage(Request clientRequest) throws IOException {
        Object msg = clientRequest.getDataByKey("msg");
        Response response = new Response(ResponseStatus.OK, ResponseType.SEND_MESSAGE);
        response.addData("msg", msg);
        if (msg instanceof GroupMessage){  //Ⱥ����Ϣ
            for(Map.Entry<String, ClientIO> receiveUser : onlineUserIO.entrySet())  //�����������û�������Ӧ
                if (!receiveUser.getKey().equals(((GroupMessage) msg).getSendUserID()))  //�������Լ���Map.Entry��Map������һ���ڲ��ӿڣ��˽ӿ�Ϊ���ͣ�����ΪEntry<K,V>
                    sendResponse(response, receiveUser.getValue().getOos()); 
            feedbackArea.append("--------Ⱥ��Ϣ--------\n");
            feedbackArea.append(msg.toString());
            feedbackArea.append("---------------------\n");
        } else if(msg instanceof P2PMessage){	//˽����Ϣ
            String receiveUserID = ((P2PMessage) msg).getReceiveUserID();
            sendResponse(response, onlineUserIO.get(receiveUserID).getOos());//��˽�Ķ�������Ӧ
        }
    }
    
    //����˷�����Ӧ����
    public void sendResponse(Response response, ObjectOutputStream oos) throws IOException {
        oos.writeObject(response);  //������д������
        oos.flush();  //ˢ����
    }
    
    //�����û�����
    public void dealLogout(Request clientRequest, ClientIO userIO) throws IOException{
        String logoutUserID = (String) clientRequest.getDataByKey("userID");
        UserInfo userInfo = regUsersInfo.get(logoutUserID);
        System.out.println("IDΪ" + userInfo.getUserID() + "���û�����");
        onlineUserIO.remove(userInfo.getUserID());//�ѵ�ǰ���߿ͻ��˵�IO��Map��ɾ��
        userIO.getOos().close();
        userIO.getOis().close();
        removeUserFromOnlineUsersTable(userInfo.getUserID());

        Response response = new Response(ResponseStatus.OK, ResponseType.FRIEND_LOGOUT);
        response.addData("userInfo", userInfo);
        //��ȡ���е����ߺ��ѣ����û�����
        ArrayList<UserInfo> friends = friendShip.get(userInfo.getUserID());
        if (friends != null) {
            for (UserInfo user : friends) 
                if (onlineUserIO.containsKey(user.getUserID()))  //��ǰ��������,��֪ͨ�ú���
                    sendResponse(response, onlineUserIO.get(user.getUserID()).getOos());
        }
    }
    
    //������Ӻ���
    public void dealAddFriend(Request clientRequest, ClientIO userIO) throws IOException {
        String userID = (String) clientRequest.getDataByKey("userID");
        String fromUserID = (String) clientRequest.getDataByKey("fromUserID");
        Response response;
        if (!regUsersInfo.containsKey(userID)){//û�д��û�
            response = new Response(ResponseStatus.OK, ResponseType.WRONG_ID);
            sendResponse(response, userIO.getOos());
        } else {
        	friendShip.get(userID).add(regUsersInfo.get(fromUserID));
        	friendShip.get(fromUserID).add(regUsersInfo.get(userID));
            //�����󷽻�Ӧ
            response = new Response(ResponseStatus.OK, ResponseType.SUCCESS_ADD);
            response.addData("userInfo", regUsersInfo.get(userID));
            response.addData("isOnline", onlineUserIO.containsKey(userID));
            sendResponse(response, userIO.getOos());
            //֪ͨ����ӷ�
            if (onlineUserIO.containsKey(userID)) {
                response = new Response(ResponseStatus.OK, ResponseType.ADD_FRIEND);
                response.addData("userInfo", regUsersInfo.get(fromUserID));
                sendResponse(response, onlineUserIO.get(userID).getOos());
            }
        }
    }
    
    //ǿ������
    public void removeUser(String userID) throws IOException{
        Response response = new Response(ResponseStatus.OK, ResponseType.FORCED_OFFLINE);
        response.addData("notice", "ϵͳ֪ͨ������ǿ�����ߣ�");
        ObjectOutputStream oos = onlineUserIO.get(userID).getOos();
        sendResponse(response, oos);//������Ӧ
        onlineUserIO.remove(userID);//�ѵ�ǰ���߿ͻ��˵�IO��Map��ɾ��
        response = new Response(ResponseStatus.OK, ResponseType.FRIEND_LOGOUT);
        response.addData("userInfo", regUsersInfo.get(userID));
        //��ȡ���е����ߺ��ѣ����û�����
        ArrayList<UserInfo> friends = friendShip.get(userID);
        if (friends != null) {
            for (UserInfo user : friends) 
                if (onlineUserIO.containsKey(user.getUserID()))  //��ǰ��������,��֪ͨ�ú���
                    sendResponse(response, onlineUserIO.get(user.getUserID()).getOos());
        }
    }
    
    //Ⱥ��ϵͳ��Ϣ
    public void massTexting(String sysMessage, JTextArea feedbackArea) throws IOException {
        Response response = new Response(ResponseStatus.OK, ResponseType.SYSTEM_NOTICE);
        response.addData("notice", sysMessage);
        for (Map.Entry<String, ClientIO> user : onlineUserIO.entrySet())
            sendResponse(response, user.getValue().getOos());    
        feedbackArea.append("-------������Ⱥ��-------\n");
        feedbackArea.append("Ⱥ���ɹ�������" + onlineUserIO.size() + "�������û�����ϵͳ��Ϣ��\n");
        feedbackArea.append("----------------------\n");
    }
    
    //���浱ǰ����������
    public void saveServerData(){
        File file = new File("D:\\Eclipse\\JavaSocket������\\src\\Server\\Service\\database.dat");
        FileOutputStream out;
        try {
            out = new FileOutputStream(file);
            ObjectOutputStream objOut = new ObjectOutputStream(out);
            objOut.writeObject(regUsersInfo);
            objOut.writeObject(usedNickname);
            objOut.writeObject(friendShip);
            objOut.writeObject(null);
            objOut.flush();
            objOut.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    
    //���ط���������
    private void loadServerData(){
        File file = new File("D:\\Eclipse\\JavaSocket������\\src\\Server\\Service\\database.dat");
        try{
            FileInputStream in = new FileInputStream(file);
            ObjectInputStream objIn = new ObjectInputStream(in);
            regUsersInfo = (HashMap) objIn.readObject();//��ע���û���Ϣ
            usedNickname = (HashSet)objIn.readObject();//��ʹ���û��ǳ�
            friendShip = (HashMap) objIn.readObject();//���ѹ�ϵ
            objIn.close();
            in.close();
        } catch (ClassNotFoundException |IOException ignore) {
        }
        if (regUsersInfo == null)
            regUsersInfo = new HashMap<>();   
        if (usedNickname == null)
            usedNickname = new HashSet<>();
        if (friendShip == null)
        	friendShip = new HashMap<>();
    }
    
    //�û����ߣ��������û�����ͼ���Ƴ��û�
    private void removeUserFromOnlineUsersTable(String userID){
        Vector vector = onlineUsersTableModel.getDataVector();
        int size = vector.size();
        int targetRowIndex = -1;
        for (int i = 0; i < size; i++) {
            Vector user = (Vector) vector.get(i);
            if (user.get(0).equals(userID)) {
                targetRowIndex = i;
                break;
            }
        }
        onlineUsersTableModel.removeRow(targetRowIndex);
    }
    
    //�������û�����ͼ������û�
    private void addUserToOnlineUsersTable(String userID, String nickname){
        onlineUsersTableModel.addRow(new String[]{userID, nickname});
    }
    public void setOnlineUsersTableModel(DefaultTableModel onlineUsersTableModel) {
        this.onlineUsersTableModel = onlineUsersTableModel;
    }
    public void setFeedbackArea(JTextArea feedbackArea) {
        this.feedbackArea = feedbackArea;
    }
}
