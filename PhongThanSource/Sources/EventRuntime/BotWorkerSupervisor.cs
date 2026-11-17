namespace PhongThan.Event.Runtime;
public static class BotWorkerSupervisor {
    public static async Task Run(Func<CancellationToken,Task> worker,Action<Exception> fault,CancellationToken stop){
        while(!stop.IsCancellationRequested){try{await worker(stop);if(!stop.IsCancellationRequested)throw new InvalidOperationException("Bot chat worker stopped before service cancellation.");}
            catch(OperationCanceledException)when(stop.IsCancellationRequested){return;}
            catch(Exception e){fault(e);}
            await Task.Delay(1000,stop);
        }
    }
}
