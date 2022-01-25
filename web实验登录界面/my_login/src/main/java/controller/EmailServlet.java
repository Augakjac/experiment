package controller;

import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang3.RandomStringUtils;

import com.alibaba.fastjson.JSON;

import modle.JsonResult;
import modle.MailUtil;
import net.sf.json.JSONObject;

/**
 * �����ּ򵥵�Servlet
 */
public class EmailServlet extends HttpServlet {
	private static final long serialVersionUID = 1L;

	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		this.doPost(request, response);
	}

	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        // ��ȡ����
		String email = request.getParameter("email");
        System.out.println(email);
        // ��ȡ�����6λ��
        String num = RandomStringUtils.randomNumeric(6);
        // �ʼ�����
        String emailMsg = "������֤��Ϊ��"+ num +"����й©�����ˣ�";
        
        // �ڷ������˱����ʼ���֤��
        request.getSession().setAttribute("emailCode", num);
        
        JsonResult jr = new JsonResult();
        
        try {
        	MailUtil mail = new MailUtil();
			// �����ʼ�
        	mail.sendMail(email, emailMsg);
        	jr.setType(1); // ���ͳɹ�
        	response.getWriter().write(JSON.toJSONString(jr));
        	return;
		} catch (Exception e) {			
			e.printStackTrace();
			jr.setType(0); // ����ʧ��
			jr.setError("�ʼ�����ʧ��");
			response.getWriter().write(JSON.toJSONString(jr));
        	return;
		}
	}

}
