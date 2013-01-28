
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.util.concurrent.atomic.AtomicInteger;


public class TestClient {

    private static final byte LINK = 1;
    private static final byte SUBMIT = 3;
    private static final byte SESSION_CLOSE_REQ = 5;
    private static AtomicInteger serial = new AtomicInteger(100);
    
    private static ByteBuffer buildLinkReq(String cin) throws IOException {
        ByteBuffer data = ByteBuffer.allocate(30);
        int reqid = serial.incrementAndGet();
        // header
        data.put((byte)1);
        data.putInt(reqid);
        data.put(LINK);
        data.putShort((short)22);
        
        // body
        if (cin == null)
            data.put(new byte[]{0,0,0,0});
        else
            data.put(cin.getBytes());
        data.put("0123456789abcdef".getBytes());
        data.putShort((short)1);
        
        return data;
    }

    private static ByteBuffer buildSubmitReq() throws IOException {
        ByteBuffer data = ByteBuffer.allocate(38);
        int reqid = serial.incrementAndGet();
        // header
        data.put((byte)1);
        data.putInt(reqid);
        data.put(SUBMIT);
        data.putShort((short)30);
        
        // body
        data.put((byte)0); // no dup
        data.putShort((short)1); // rec count
        data.putShort((short)0); // interval
        data.put("2012010203040500".getBytes()); // timestamp, 16bit??
        data.put((byte)8);
        data.put("87654321".getBytes());
        
        return data;
    }
    
    private static ByteBuffer buildCloseReq() throws IOException {
        ByteBuffer data = ByteBuffer.allocate(8);
        int reqid = serial.incrementAndGet();
        // header
        data.put((byte)1);
        data.putInt(reqid);
        data.put(SESSION_CLOSE_REQ);
        data.putShort((short)0);
        
        return data;
    }
    public static void main(String[] args) throws Exception {
        String host = args[0];
        int port = Integer.parseInt(args[1]);
        int count = Integer.parseInt(args[2]);
        
        long start = System.currentTimeMillis();
        Socket s = new Socket(host, port);
        InputStream in = s.getInputStream();
        OutputStream out = s.getOutputStream();

        ByteBuffer data = buildLinkReq("1234");
        
        byte[] header = new byte[8];
        out.write(data.array());
        in.read(header);
        ByteBuffer resh = ByteBuffer.wrap(header);
        resh.position(6);
        int length = resh.getShort();
        byte[] body = new byte[length];
        in.read(body);
        
        assert (body[0] == 0);

        for (int i =0; i < count; i++) {
            data = buildSubmitReq();
            out.write(data.array());
            in.read(header);
            resh = ByteBuffer.wrap(header);
            resh.position(6);
            length = resh.getShort();
            body = new byte[length];
            in.read(body);
            assert (body[0] == 0);
        }
        // close
        data = buildCloseReq();
        out.write(data.array());
        in.read(header);
        resh = ByteBuffer.wrap(header);
        resh.position(6);
        length = resh.getShort();
        
        assert(length == 0);
        s.close();
        
        long elapse = System.currentTimeMillis() - start;
        long per = ( count * 1000L / elapse ) ;

        System.out.println("Elapsed " + elapse + " ms, " + per + " messages/s");
    }

}
