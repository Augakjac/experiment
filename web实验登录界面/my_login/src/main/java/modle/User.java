package modle;

/**
 * 
 * @author С��
 * User��Ϊ�û�������Ϣ����
 *
 */
public class User {
	private String username;
	private String password;
	
	// ���캯��
	public User() {}
	 
	public User(String username, String password) {
		this.username = username;
		this.password = password;
	}
	
	// ��ȡ�û���
	public String getUsername() {
		return username;
	}
	
	// �����û���
	public void setUsersname(String username) {
		this.username = username;
	}
	
	// ��ȡ����
	public String getPassword() {
		return password;
	}
	
	// ��������
	public void setPassword(String password) {
		 this.password = password;
	}
}
