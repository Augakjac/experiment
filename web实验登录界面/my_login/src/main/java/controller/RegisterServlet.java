package controller;

import java.io.IOException;
import java.io.PrintWriter;
import java.sql.Connection;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import modle.DAO;
import modle.JDBCUtil;
import modle.User;

/**
 * ע���servlet
 */
public class RegisterServlet extends HttpServlet {
	private static final long serialVersionUID = 1L;

	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {		
		this.doPost(request, response);
	}

	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		
		// ��ȡע����������
		String username = request.getParameter("username").trim();
        String password = request.getParameter("password").trim();
        String again_password = request.getParameter("again_password").trim();
        
        //��������ַ�����
        byte[] bytes = username.getBytes("ISO-8859-1");
        username = new String(bytes,"utf-8");
        
        response.setContentType("text/html;charset=UTF-8");
        PrintWriter out = response.getWriter();
        
        JDBCUtil db = new JDBCUtil();
        // ����һ���û������½�������û�������
        User user = new User(username,password);
        DAO dao = new DAO();
        
        try {
        	//���ݿ�����
            Connection conn = db.getConn();
            
            if(!password.equals(again_password)) {
            	out.println("<h2>������������벻һ��</h2>");
	        	out.println("<h2>3���Ժ󷵻�ע��ҳ��</h2>");
            	response.setHeader("Refresh", "3;url=jsp/register.jsp");
            } else {
            	if(dao.register(conn, user)) {
            		out.println("<h1>ע�����û��ɹ�</h1>");
                	out.println("<h2>3���Ժ���ת��ע��ҳ��</h2>");
                	response.setHeader("Refresh", "3;url=jsp/login.jsp");                
                } else {
                	out.println("<h1>ע�����û�ʧ��,�û����Ѿ�����</h1>");
                	out.println("<h2>3���Ժ���ת��ע��ҳ��</h2>");
                	response.setHeader("Refresh", "3;url=jsp/register.jsp");
                }
            }            
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			out.close();
		}
	}

}
