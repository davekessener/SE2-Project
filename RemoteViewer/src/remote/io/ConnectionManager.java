package remote.io;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;

public class ConnectionManager
{
	private final ServerSocket mSocket;
	private final Thread mListenerThread;
	private final List<NetReader> mClients;
	private final IAcceptor mCallback;
	private boolean mRunning;
	
	public ConnectionManager(int port, IAcceptor f) throws IOException
	{
		mSocket = new ServerSocket(port);
		mClients = new ArrayList<>();
		mCallback = f;
		mRunning = true;
		
		System.out.println("Listening on port " + port);
		
		mListenerThread = new Thread(() -> listen());
		mListenerThread.start();
	}
	
	public void stop()
	{
		try
		{
			mSocket.close();
			
			for(NetReader t : mClients)
			{
				t.stop();
			}
			
			mListenerThread.interrupt();
			mListenerThread.join();
		}
		catch (InterruptedException | IOException e)
		{
		}
		
		System.out.println("Shutting down ...");
	}
	
	private void listen()
	{
		try
		{			
			while(mRunning)
			{
				mClients.add(new NetReader(mSocket.accept()));
			}
		}
		catch (IOException e)
		{
		}
	}
	
	private class NetReader
	{
		private final Socket mSocket;
		private final BufferedReader mRead;
		private final Thread mThread;
		
		public NetReader(Socket s) throws IOException
		{
			mSocket = s;
			mRead = new BufferedReader(new InputStreamReader(s.getInputStream()));
			mThread = new Thread(() -> readLines());
			
			System.out.println("Opened a connection!");
			mThread.start();
		}
		
		public void stop()
		{
			try
			{
				mSocket.close();
				mThread.interrupt();
				mThread.join();
			}
			catch(IOException | InterruptedException e)
			{
			}
		}
		
		private void readLines()
		{
			try
			{
				while(!mSocket.isClosed())
				{
					String line = mRead.readLine();
					
					if(line == null) break;
					
					mCallback.accept(line);
				}
			}
			catch (IOException e)
			{
			}
		}
	}
	
	public static interface IAcceptor { void accept(String s); }
}
