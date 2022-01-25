package controller;

import java.io.IOException;
import java.io.PrintWriter;
import java.sql.Connection;
import java.sql.PreparedStatement;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import modle.DAO;
import modle.JDBCUtil;
import modle.User;

/**
 * �޸������ servlet
 */
public class HandlePwdServlet extends HttpServlet {
	private static final long serialVersionUID = 1L;

	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		this.doPost(request, response);
	}

	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		// ��ȡ����
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
            // ���ݿ���û�и��û�
            if(dao.searchUser(conn, user) == null) {
	        	out.println("<h2>���û������ڣ�����ȥע��</h2>");
	        	out.println("<h2>3���Ժ󷵻��޸�����ҳ��</h2>");
            	response.setHeader("Refresh", "3;url=jsp/change_pwd.jsp");
            } else {
				if(!password.equals(again_password)) {
					out.println("<h2>������������벻һ��</h2>");
		        	out.println("<h2>3���Ժ󷵻��޸�����ҳ��</h2>");
	            	response.setHeader("Refresh", "3;url=jsp/change_pwd.jsp");
				} else {
					String sql="update users set password=? where username=?";
			        // ���ִ��sql���Ķ���
			        PreparedStatement pstatement =conn.prepareStatement(sql);
			        pstatement.setString(1, user.getPassword());
			        pstatement.setString(2, user.getUsername());
			        // ������Ӱ���޸ĵ�����
			        int res = pstatement.executeUpdate();
			        if(res != 0) {
			        	out.println("<h1>�޸�����ɹ�</h1>");
		            	out.println("<h2>3���Ժ���ת�ص�¼ҳ��</h2>");
		            	response.setHeader("Refresh", "3;url=jsp/login.jsp");
			        } else {
			        	out.println("<h2>�޸�����ʧ��</h2>");
			        	out.println("<h2>3���Ժ󷵻��޸�����ҳ��</h2>");
		            	response.setHeader("Refresh", "3;url=jsp/change_pwd.jsp");
					}
				}				
			}
        } catch (Exception e) {
        	e.printStackTrace();
		} finally {
			out.close();
		}
	}

}
