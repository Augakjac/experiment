package Reptile;

import Frame.MainFrame;
import java.io.*;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;
import java.util.regex.Pattern;

public class Reptile extends Thread {
	private ArrayList<String> urls; // ��ҳurl
	private boolean saveAsFile;
	public ArrayList<String> result; // ��ȡ���

	public Reptile(ArrayList<String> urls, boolean saveAsFile) {
		this.urls = urls;
		this.saveAsFile = saveAsFile;
	}

	public ArrayList<String> crawl() {
		ArrayList<String> html = new ArrayList<>(); // ����ÿ��ҳ����ȡ��html
		try {
			for (int i = 0; i < urls.size(); i++) {
				URL url = new URL(urls.get(i));
				HttpURLConnection urlConnect = (HttpURLConnection) url.openConnection();
				urlConnect.setDoInput(true);
				urlConnect.setRequestProperty("User-Agent","Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.114 Safari/537.36 Edg/91.0.864.54");
				urlConnect.setRequestMethod("GET");

				InputStream input = urlConnect.getInputStream();// ��ȡ������
				BufferedReader br = new BufferedReader(new InputStreamReader(input, "utf-8"));

				StringBuilder tmpSb = new StringBuilder();
				String line = br.readLine();
				tmpSb.append(line);
				while (line != null) {
					line = br.readLine();
					if (line != null)
						tmpSb.append(line);
				}
				html.add(tmpSb.toString());
				MainFrame.htmlArea.append(tmpSb + "\n");
			}
		} catch (Exception e) {
			e.printStackTrace();
		}

		if (saveAsFile) {
			try {
				File f = new File("content.txt");
				f.createNewFile();
				BufferedWriter bw = new BufferedWriter(new FileWriter(f));
				StringBuilder parsedContent = new StringBuilder();
				for (int i = 0; i < html.size(); i++) { // ��ԭʼhtml�е��ı���ȡ����
					parsedContent.append("---------��ҳ:" + urls.get(i) + "-----------");
					MainFrame.textArea.append("---------��ҳ:" + urls.get(i) + "-----------\n");
					parsedContent.append(parseHtml(html.get(i)));
					MainFrame.textArea.append(parseHtml(html.get(i)) + "\n");
					parsedContent.append("\n");
					MainFrame.textArea.append("\n");
				}
				bw.write(parsedContent.toString());
				bw.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return html;
	}

	// ��ԭʼHTMLҳ���ǩ�е��ı���ȡ����
	public String parseHtml(String html) {
		String result = "";
		Pattern script;
		Pattern style;
		Pattern label;
		Pattern escChar;
		try {
			String jsPattern = "<[\\s]*?script[^>]*?>[\\s\\S]*?<[\\s]*?/[\\s]*?script[\\s]*?>"; // js
			String cssPattern = "<[\\s]*?style[^>]*?>[\\s\\S]*?<[\\s]*?/[\\s]*?style[\\s]*?>"; // css
			String labelPattern = "<[^>]+>"; // ��ǩ<>
			String escCharPattern = "&.*?;"; // ת���ַ�
			script = Pattern.compile(jsPattern, Pattern.CASE_INSENSITIVE);
			html = script.matcher(html).replaceAll("");// ȥ��js
			style = Pattern.compile(cssPattern, Pattern.CASE_INSENSITIVE);
			html = style.matcher(html).replaceAll("");// ȥ��css
			label = Pattern.compile(labelPattern, Pattern.CASE_INSENSITIVE);
			html = label.matcher(html).replaceAll(" ");// ȥ����ǩ
			escChar = Pattern.compile(escCharPattern, Pattern.CASE_INSENSITIVE);
			html = escChar.matcher(html).replaceAll("");// ȥ��ת���ַ�
			result = html;
		} catch (Exception e) {
			e.printStackTrace();
		}
		result = result.replaceAll("[ ]+", " ");// ����հ��ַ���Ϊһ���ո�
		result = result.replaceAll("[\\t]+", " ");
		return result;
	}

	@Override
	public void run() {
		result = crawl();
	}
}
