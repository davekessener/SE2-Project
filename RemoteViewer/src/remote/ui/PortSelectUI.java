package remote.ui;

import javafx.geometry.HPos;
import javafx.geometry.Insets;
import javafx.geometry.VPos;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.Priority;
import javafx.scene.text.Font;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;

public class PortSelectUI extends Stage
{
	private final ISelectHandler mCallback;
	private final Runnable mOnCancel;
	private boolean mCanClose;
	
	public PortSelectUI(ISelectHandler h, Runnable on_close)
	{
		mCallback = h;
		mOnCancel = on_close;
		mCanClose = false;
		
		this.setOnCloseRequest(e -> {
			if(!mCanClose)
			{
				mOnCancel.run();
			}
		});
		
		setResizable(false);
		setScene(createScene());
		setTitle("Select port");
	}
	
	private Scene createScene()
	{
		GridPane root = new GridPane();
		GridPane btns = new GridPane();
		
		Label lbl = new Label("Listen on port");
		Label err = new Label("");
		TextField input = new TextField("8000");
		
		Button ok = new Button("OK");
		Button cancel = new Button("Cancel");
		
		err.setFont(Font.font(err.getFont().getName(), 10));
		
		ok.setOnAction(e -> {
			try
			{
				if(mCallback.select(Integer.parseInt(input.getText())))
				{
					this.close();
				}
				else
				{
					err.setText("Couldn't open port!");
				}
			}
			catch(NumberFormatException ex)
			{
				err.setText("Invalid number!");
			}
		});

		ok.setPrefWidth(80);
		cancel.setPrefWidth(80);
		input.setPrefWidth(200);
		
		cancel.setOnAction(e -> fireEvent(new WindowEvent(this, WindowEvent.WINDOW_CLOSE_REQUEST)));
		
		btns.addRow(0, err, ok, cancel);

		root.addRow(0, lbl, input);
		root.add(btns, 0, 1, 2, 1);
		
		GridPane.setHgrow(err, Priority.ALWAYS);
		GridPane.setValignment(err, VPos.BOTTOM);
		GridPane.setHalignment(btns, HPos.RIGHT);
		GridPane.setHalignment(ok, HPos.RIGHT);
		GridPane.setHalignment(cancel, HPos.RIGHT);
		
		btns.setHgap(6);
		root.setHgap(8);
		root.setVgap(8);
		
		root.setPadding(new Insets(10, 0, 0, 10));
		
		return new Scene(root);
	}
	
	public static interface ISelectHandler { boolean select(int port); }	
}
