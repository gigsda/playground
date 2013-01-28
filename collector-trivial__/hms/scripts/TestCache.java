
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.util.concurrent.atomic.AtomicInteger;


public class TestCache {

    private static final byte LINK = 1;
    private static final byte SUBMIT = 3;
    private static final byte SESSION_CLOSE_REQ = 5;
    private static final byte LAST_REQ = 7;
    private static AtomicInteger serial = new AtomicInteger(100);
    
    private static ByteBuffer buildLastReq(int type, int value) throws IOException {
        ByteBuffer data = ByteBuffer.allocate(13);
        int reqid = serial.incrementAndGet();
        // header
        data.put((byte)1);
        data.putInt(reqid);
        data.put(LAST_REQ);
        data.putShort((short)5);
        
        // body
        data.put((byte)type); 
        data.putInt(value); 
        
        return data;
    }
    
    public static void main(String[] args) throws Exception {
        String host = args[0];
        int port = Integer.parseInt(args[1]);
        int type = Integer.parseInt(args[2]);
        int count = Integer.parseInt(args[3]);
        
        Socket s = new Socket(host, port);
        InputStream in = s.getInputStream();
        OutputStream out = s.getOutputStream();

        ByteBuffer data = buildLastReq(type, count);
        
        byte[] header = new byte[8];
        out.write(data.array());
        in.read(header);
        ByteBuffer resh = ByteBuffer.wrap(header);
        resh.position(6);
        int length = resh.getShort();
        if (length < 0)
            length = 0xffff & length;
        System.out.println("Length " + length);
        byte[] body = new byte[length];
        in.read(body);
        for (int i=0; i < Math.min(length,8); i++) 
            System.out.print(String.format("%02X ",body[i]));
        
        System.out.println();
        s.close();
        
    }

}
