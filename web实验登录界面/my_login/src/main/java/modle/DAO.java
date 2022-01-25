package modle;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;

/**
 * 
 * @author С��
 * DAO ��Ϊ�������ݿ��ѯ�û��Ĺ���
 * 
 *
 */

public class DAO {
	// ��¼��֤
	public User login(Connection conn,User user) throws Exception{
        User resultUser = null;
        // sql ��ѯ���
        String sql="select * from users where username=? and password=?";
        // ���ִ��sql���Ķ���
        PreparedStatement pstatement =conn.prepareStatement(sql);
        pstatement.setString(1, user.getUsername());
        pstatement.setString(2, user.getPassword());
        // ִ��sql����ý����
        ResultSet rs = pstatement.executeQuery();
        if(rs.next()){ 
            resultUser = new User();
            resultUser.setUsersname(rs.getString("username"));
            resultUser.setPassword(rs.getString("password"));
        }
        
        return resultUser;
    }
	
	// �޸���������û�
	public User searchUser(Connection conn,User user) throws Exception {
		User resultUser = null;
        // sql ��ѯ���
        String sql="select * from users where username=?";
        // ���ִ��sql���Ķ���
        PreparedStatement pstatement =conn.prepareStatement(sql);
        pstatement.setString(1, user.getUsername());
        // ִ��sql����ý����
        ResultSet rs = pstatement.executeQuery();
        if(rs.next()){ 
            resultUser = new User();
            resultUser.setUsersname(rs.getString("username"));
        }
        
        return resultUser;
	}
	
	// ע����֤
	public boolean register(Connection conn,User user) throws Exception {
		boolean flag = false;
        // sql ��ѯ���
        String sql="insert into users(username,password)values(?,?)";
        // ���ִ��sql���Ķ���
        PreparedStatement pstatement =conn.prepareStatement(sql);
        pstatement.setString(1, user.getUsername());
        pstatement.setString(2, user.getPassword());
        // ִ��sql����ý����
        int res = pstatement.executeUpdate();
        if(res > 0) {
        	flag = true;
        }
        return flag;
	}
	
}
