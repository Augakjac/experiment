package modle;

import java.util.Properties;

import javax.mail.Authenticator;
import javax.mail.Message;
import javax.mail.PasswordAuthentication;
import javax.mail.Session;
import javax.mail.Transport;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeMessage;

public class MailUtil {
	/**
	 * 
	 * @param email ��¼�û����ʼ�
	 * @param emailMsg  ���͵��ʼ���Ϣ
	 * @throws Exception
	 */
	public void sendMail(String userEmail, String emailMsg) throws Exception {
		
        // 1. ����һ���ʼ�������һ���������ʼ��������Ự����session
        Properties props = new Properties();
        props.setProperty("mail.transport.protocol", "SMTP");
        props.setProperty("mail.host", "smtp.126.com"); //smtp.126.comΪSMTP��������ַ��Ϊָ����������������ʼ�
        props.setProperty("mail.smtp.auth", "true"); // ָ����֤Ϊtrue
        
        // ������֤��
        Authenticator auth = new Authenticator() {
        	public PasswordAuthentication getPasswordAuthentication() {
				return new PasswordAuthentication("xxx", "��Ȩ����"); //��һ������Ϊ��Ȩ����
			}
        };
        
        // ���������ʼ��������Ĳ������ã������ʼ�ʱ��Ҫ�õ���
        Session session= Session.getInstance(props,auth);  // ���ݲ������ã������Ự����Ϊ�˷����ʼ�׼���ģ�
        
        
        // 2.�����ʼ�����message���൱���ʼ�����
        Message message = new MimeMessage(session);

        // From: ������
        // ���� InternetAddress �����������ֱ�Ϊ: ����, ��ʾ���ǳ�(ֻ������ʾ, û���ر��Ҫ��), �ǳƵ��ַ�������
        // ����Ҫ����ʱ, �����������ʵ��Ч�����䡣
        message.setFrom(new InternetAddress("�����ַ"));  

        // To: �ռ��� �����ռ��˺ͷ��ͷ�ʽ
        message.setRecipient(MimeMessage.RecipientType.TO, new InternetAddress(userEmail));
      
        // Subject: �ʼ�����
        message.setSubject("������֤");

        // Content: �ʼ����ģ�����ʹ��html��ǩ��
        message.setContent(emailMsg, "text/html;charset=UTF-8");

        // 3. ���� transport ���ڽ��ʼ�����
        Transport.send(message);
    }
}
