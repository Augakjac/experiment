package modle;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.Statement;

/**
 * @author С��
 * JDBCUtil �������: jdbc ��һЩ������
 *
 */

public class JDBCUtil {
	// ���ݿ�Ĳ���
	private String dbUrl="jdbc:mysql://localhost:3306/my_login?useSSL=false";
    private String dbUsername="root";
    private String dbPassword="201429";
    
    // �����ݿ�����
    public Connection getConn() {
    	try {
    		// ��������
    		Class.forName("com.mysql.jdbc.Driver");
		} catch (Exception e) {
			e.printStackTrace();
		}
    	Connection conn = null;
    	
    	try {
    		// �������,����connection ����
    		conn = DriverManager.getConnection(dbUrl, dbUsername, dbPassword);
		} catch (Exception e) {
			e.printStackTrace();
		}
    	return conn;
    }
    
    // �ͷ���Դ
    // �رս���� ResultSet
    public void close(ResultSet resultSet) throws Exception {
		if(resultSet != null) {
			resultSet.close();
		}
	}
    
    // �ر� sql ������ Statement
    public void close(Statement statement) throws Exception {
    	if(statement != null) {
    		statement.close();
    	}
    }
    
    // �ر����Ӷ��� Connection
    public void close(Connection conn) throws Exception {
    	if(conn != null) {
    		conn.close();
    	}
    }
        
//    public static void main(String[] args) {
//    	JDBCUtil dbUtil = new JDBCUtil();
//        try {
//        	Connection conn = dbUtil.getConn();
//        	System.out.println("���ݿ����ӳɹ�");
//        	Statement statement = conn.createStatement();
//    		// 3.2 ��дsql���
//    		String sql = "select * from users";
//    		// 3.3 ִ��sql���
//    		ResultSet rs = statement.executeQuery(sql);
//    		// 3.4 ���������
//    		while(rs.next()) {
//    			System.out.print(rs.getInt("id")+" ");
//    			System.out.print(rs.getString("username")+" ");
//    			System.out.print(rs.getString("password")+" ");
//    			System.out.println();
//    		}
//    		dbUtil.close(rs);
//    		dbUtil.close(statement);
//    		dbUtil.close(conn);
//    		
//        } catch (Exception e) {            
//            e.printStackTrace();
//        }
//	}
}
