package Server.Frame;

import Server.Service.Server;

import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.awt.event.*;
import java.io.IOException;

public class ServerFrame extends JFrame {
    Server server;
    public ServerFrame(Server server){
        this.server = server;
        this.init();
    }
    private void init(){
        this.setTitle("������");
        this.setSize(1000, 670);
        //����Ĭ�ϴ�������Ļ����
        int x = (int) Toolkit.getDefaultToolkit().getScreenSize().getWidth();
        int y = (int) Toolkit.getDefaultToolkit().getScreenSize().getHeight();
        this.setLocation((x - this.getWidth()) / 2, (y - this.getHeight()) / 2);
        this.setResizable(false);

        //�����û���ʾ��
        JPanel onlineUsersPanel = new JPanel();
        Border border = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);
        onlineUsersPanel.setBorder(BorderFactory.createTitledBorder(border, "�����û�", TitledBorder.CENTER,TitledBorder.CENTER));
        ((TitledBorder) onlineUsersPanel.getBorder()).setTitleFont(new Font("����", Font.BOLD, 16));
        onlineUsersPanel.setPreferredSize(new Dimension(450, 650));
        this.add(onlineUsersPanel, BorderLayout.WEST);
        
        //���������û��������
        JTable onlineUsersTable = new JTable();
        onlineUsersTable.setRowHeight(30); //���ñ��ÿ�и߶�
        onlineUsersTable.setEnabled(false); //���ñ�񲻿ɱ���д����
        DefaultTableCellRenderer r = new DefaultTableCellRenderer(); //���ñ�����ݾ���
        r.setHorizontalAlignment(JLabel.CENTER);
        onlineUsersTable.setDefaultRenderer(Object.class, r);

        DefaultTableModel onlineUsersTableModel = new DefaultTableModel();
        String[] onlineUsersTableColumnNames = {"�û�id", "�ǳ�"};
        Object[][] onlineUsersTableData = {};
        onlineUsersTableModel.setDataVector(onlineUsersTableData, onlineUsersTableColumnNames);
        onlineUsersTable.setModel(onlineUsersTableModel);
        server.setOnlineUsersTableModel(onlineUsersTableModel);
        JScrollPane onlineUsersTableScroll = new JScrollPane(onlineUsersTable);
        onlineUsersTableScroll.setPreferredSize(new Dimension(440, 600));
        onlineUsersTableScroll.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
        onlineUsersPanel.add(onlineUsersTableScroll, BorderLayout.CENTER);
        
        //Ϊ�����û��������Ҽ��������
        onlineUsersTable.addMouseListener(new MouseAdapter() {
            public void mousePressed(MouseEvent me) {
                if (SwingUtilities.isRightMouseButton(me)) {
                    final int row = onlineUsersTable.rowAtPoint(me.getPoint());
                    if(row != -1){
                        final JPopupMenu popUp = new JPopupMenu();
                        JMenuItem select = new JMenuItem("ǿ������");
                        select.addActionListener(new ActionListener() {
                            public void actionPerformed(ActionEvent e) {
                                try {
                                    server.removeUser((String) onlineUsersTableModel.getValueAt(row, 0));
                                } catch (IOException ignore) {
                                }
                                onlineUsersTableModel.removeRow(row);
                            }
                        });
                        popUp.add(select);
                        popUp.show(me.getComponent(), me.getX(), me.getY());
                    }
                }
            }
        });
        
        //ϵͳȺ����Ϣ��
        JPanel sysBatchSendPanel = new JPanel();
        sysBatchSendPanel.setBorder(BorderFactory.createTitledBorder(border, "״̬��Ϣ", TitledBorder.CENTER,TitledBorder.CENTER));
        ((TitledBorder) sysBatchSendPanel.getBorder()).setTitleFont(new Font("����", Font.BOLD, 16));
        sysBatchSendPanel.setPreferredSize(new Dimension(750, 650));
        this.add(sysBatchSendPanel, BorderLayout.CENTER);
        
        //��Ϣ����״̬��
        JTextArea feedbackArea = new JTextArea();
        feedbackArea.setEditable(false);
        feedbackArea.setBackground(Color.lightGray);
        feedbackArea.setFont(new Font("����", Font.BOLD, 15));
        feedbackArea.setLineWrap(true);        //�Զ�����
        feedbackArea.setWrapStyleWord(true);   //���в�����
        server.setFeedbackArea(feedbackArea);
        
        //����Ⱥ����Ϣ��
        JTextArea inputArea = new JTextArea();
        inputArea.setBackground(Color.lightGray);
        inputArea.setFont(new Font("����", Font.BOLD, 15));
        inputArea.setSelectedTextColor(Color.RED);
        inputArea.setLineWrap(true);        
        inputArea.setWrapStyleWord(true);       
        
        //��ť
        JButton sendBtn = new JButton("Ⱥ��");
        sendBtn.setPreferredSize(new Dimension(110, 30));
        sendBtn.setFocusPainted(false);//ȥ����
        sendBtn.setBackground(new Color(27, 127, 176));
        sendBtn.setFont(new Font("����", Font.BOLD, 16));
        sendBtn.setForeground(Color.white);
        JScrollPane feedbackAreaScroll = new JScrollPane(feedbackArea);
        feedbackAreaScroll.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);//������һֱ��ʾ
        feedbackAreaScroll.setBounds(0, 0, 600, 300);//�������ݴ�С
        feedbackAreaScroll.setPreferredSize(new Dimension(500, 300));
        JScrollPane inputAreaScroll = new JScrollPane(inputArea);
        inputAreaScroll.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
        inputAreaScroll.setBounds(0, 0, 600, 250);//�������ݴ�С
        inputAreaScroll.setPreferredSize(new Dimension(500, 250));
        sysBatchSendPanel.add(feedbackAreaScroll, BorderLayout.NORTH);
        sysBatchSendPanel.add(inputAreaScroll, BorderLayout.CENTER);
        sysBatchSendPanel.add(sendBtn, BorderLayout.SOUTH);

        //Ⱥ����ť���¼�
        sendBtn.addActionListener(e -> {
            try {
                server.massTexting(inputArea.getText(), feedbackArea);
                inputArea.setText("");
            } catch (IOException ioException) {
                ioException.printStackTrace();
            }
        });
        this.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {
                JOptionPane.showMessageDialog(ServerFrame.this, "�������ڹر�...", "�ر���ʾ", JOptionPane.INFORMATION_MESSAGE);
                server.saveServerData();
                ServerFrame.this.dispose();
                System.exit(0);
            }
        });
        this.setVisible(true);
    }
}
