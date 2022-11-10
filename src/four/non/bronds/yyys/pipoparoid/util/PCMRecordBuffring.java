package four.non.bronds.yyys.pipoparoid.util;

public class PCMRecordBuffring extends PCMRecord {

	public PCMRecordBuffring(DTFM dtfm) throws Exception {
		super(dtfm);

	}

	@Override
	public void recodStart() {


		
	}

	@Override
	public void receiveBuff(int size, byte[] buf) {

		System.out.println("receiveBuff:" + size);
		mDTFM.analyzeDTFMBuffring(buf, size, this);
		
	}

	@Override
	public void recodEnd(long elapseTime) {


		
	}
	
	
	public void readedCB(String msg) {
		System.out.println("MSG:" + msg);
	}

}
