package controller;

import java.io.IOException;
import java.io.PrintWriter;
import java.sql.Connection;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.alibaba.fastjson.JSON;

import modle.DAO;
import modle.JDBCUtil;
import modle.JsonResult;
import modle.User;

/**
 * ��¼�� Servlet
 */

public class LoginServlet extends HttpServlet {
	private static final long serialVersionUID = 1L;
       
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {		
		this.doPost(request, response);
	}

	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {				
		
		// ����ǰ̨������ֵ �˺ź����� ���� �ֻ��ź���֤��
        String username = request.getParameter("username");        
        String password = request.getParameter("password");
        String email = request.getParameter("email");
        String code = request.getParameter("code");
                
        System.out.println(password);
        System.out.println(email);
        System.out.println(code);
        
        response.setContentType("text/html;charset=UTF-8");
        PrintWriter out = response.getWriter();
        
        // �˺������¼�ķ�ʽ
        if(username != null && password != null) {
            //��������ַ�����
            byte[] bytes = username.getBytes("ISO-8859-1");
            username = new String(bytes,"utf-8");
            System.out.println(username);
            
        	JDBCUtil db = new JDBCUtil();
            // ����һ���û������½�������û�������
            User user = new User(username,password);
            DAO dao = new DAO();
            try {
                //���ݿ�����
                Connection conn = db.getConn();
                
                if(dao.login(conn, user) != null) {
                	request.getSession().setAttribute("user", user);
                	response.sendRedirect("jsp/success.jsp");
                } else {
                	out.println("<h1>�û����������������֤ʧ��</h1>");
                	out.println("<h2>3���Ժ���ת�ص�¼ҳ��</h2>");
                	response.setHeader("Refresh", "3;url=jsp/login.jsp");
                }            
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
    			out.close();
    		}
        } else if(email != null && code != null) { //������֤�뷽ʽ
        	// �ж�emailCode�Ƿ���ȷ
        	String s_emailCode = (String)request.getSession().getAttribute("emailCode");
        	JsonResult jr = new JsonResult();
        	if(!code.equalsIgnoreCase(s_emailCode)) {
            	out.println("<h1>�ʼ���֤�������֤ʧ��</h1>");
            	out.println("<h2>3���Ժ���ת�ص�¼ҳ��</h2>");
            	response.setHeader("Refresh", "3;url=jsp/login.jsp");
        	} else { // ��֤�ɹ�
        		response.sendRedirect("jsp/success.jsp");
        	}
        	out.close();
        }
        
	}

}
